/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "yellowpage.h"
#include "channelmatcher.h"
#include "application.h"
#include "mainwindow.h"
#include "network.h"
#include "settings.h"
#include "utils.h"

YellowPage::YellowPage(const QUrl &url, Type type)
    : QObject(), m_url(url), m_type(type)
{
    initialize();
}

YellowPage::YellowPage(const QString &url, Type type)
    : QObject(), m_url(url), m_type(type)
{
    initialize();
}

void YellowPage::initialize()
{
    m_enabled = true;
    m_name = tr("unnamed");
    m_usePCRawProxy = false;
    m_yellowPages += this;
    m_updating = false;
    m_lastUpdatedTime = QDateTime::currentDateTime();
    m_http = new QHttp(this);
    m_httpTimer.setSingleShot(true);
    connect(m_http, SIGNAL(done(bool)), this, SLOT(done(bool)));
    connect(&m_httpTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

YellowPage::~YellowPage()
{
    while (!m_channels.isEmpty())
        delete m_channels.takeFirst();
    while (!m_stoppedChannels.isEmpty())
        delete m_stoppedChannels.takeFirst();
}

void YellowPage::update()
{
    if (m_updating)
        return;
    m_updating = true;
    m_httpTimer.setInterval(qApp->settings()->value("Network/TimeoutInterval").toInt() * 1000);
    m_httpTimer.start();
    Network::get(m_http, channelListUrl());
}

void YellowPage::stopUpdate()
{
    if (m_updating) {
        m_http->abort();
        m_httpTimer.stop();
    }
}

bool YellowPage::isUpdating()
{
    return m_updating;
}

void YellowPage::timeout()
{
    qDebug() << "(timeout) http state: " << m_http->state();
    m_http->abort();
}

void YellowPage::done(bool error)
{
    m_httpTimer.stop();
    QHttpResponseHeader response = m_http->lastResponse();
    qDebug() << m_name << "status code:" << response.statusCode();
    if (response.statusCode() != 403 and !error) {
        ChannelList oldChannels = m_channels;
        oldChannels += m_stoppedChannels;
        m_channels.clear();
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString lines(codec->toUnicode(m_http->readAll()));
        ChannelMatcher matcher(qApp->settings());
        foreach (QString line, lines.split("\n", QString::SkipEmptyParts)) {
            QStringList fields = line.split("<>");
            //  0: チャンネル名
            //  1: チャンネルID
            //  2: tip (IP:Port)
            //  3: コンタクトURL
            //  4: ジャンル
            //  5: 詳細
            //  6: リスナー数
            //  7: リレー数
            //  8: ビットレート
            //  9: タイプ
            // 10: アーティスト名
            // 11: アルバム名
            // 12: タイトル
            // 13: 曲ジャンル
            // 14: URLエンコードされたチャンネル名
            // 15: 配信時間
            // 16: アイコンの状態
            // 17: メッセージ
            // 18: ダイレクトの有無 
            if (fields.size() < 19)
                continue;
            for (int i = 0; i < fields.count(); ++i)
                fields[i] = Utils::unescape(fields[i]);
            Channel *channel = 0;
            Channel tmpCh(fields[0]);
            tmpCh.setId(fields[1]);
            for (int i = 0; i < oldChannels.count(); ++i) {
                Channel *ch = oldChannels[i];
                if ((tmpCh.isPlayable() and ch->id() == tmpCh.id())
                        or ch->name(true) == tmpCh.name(true)) {
                    channel = oldChannels.takeAt(i);
                    if (m_stoppedChannels.contains(channel))
                        channel->setStatus(Channel::New);
                    else
                        channel->setStatus(Channel::Normal);
                    break;
                }
            }

            if (!channel)
                channel = new Channel;
            QString longDesc = channel->longDescription();
            channel->setName(fields[0]);
            channel->setId(fields[1]);
            channel->setTip(fields[2]);
            channel->setContactUrl(fields[3]);
            channel->setGenre(fields[4]);
            channel->setDescription(fields[5]);
            channel->setListeners(fields[6].toInt());
            channel->setRelays(fields[7].toInt());
            channel->setBitrate(fields[8].toInt());
            channel->setType(fields[9]);
            channel->setArtist(fields[10]);
            channel->setAlbum(fields[11]);
            channel->setTitle(fields[12]);
            // qDebug() << fields[13];
            // qDebug() << fields[14];
            channel->setUptime(fields[15].isEmpty() ? "00:00" : fields[15]);
            // qDebug() << fields[16];
            channel->setMessage(fields[17]);
            // qDebug() << fields[18];
            int score = matcher.score(channel);
            channel->setScore(score);
            if (channel->status() != Channel::New
                    and channel->name().indexOf(QRegExp("◆(Status|帯域チェック)$")) == -1
                    and longDesc != channel->longDescription())
                channel->setStatus(Channel::Changed);
            m_channels += channel;
        }

        m_stoppedChannels = oldChannels;
        foreach (Channel *ch, m_stoppedChannels) {
            ch->setStatus(Channel::Stopped);
            ch->setListeners(-2);
            ch->setRelays(-2);
        }
    } else {
        m_errorString = tr("イエローページの更新に失敗しました。(%1: %2 %3 / %4)")
            .arg(m_name).arg(response.statusCode())
            .arg(response.reasonPhrase().isEmpty() ? tr("Unknown") : response.reasonPhrase())
            .arg(m_http->errorString());
        qDebug() << m_http->lastResponse().reasonPhrase();
        qDebug() << m_http->error();
        qDebug() << m_http->errorString();
        error = true;
    }

    m_updating = false;
    m_lastUpdatedTime = QDateTime::currentDateTime();

    emit updateDone(this, error);
}

QString YellowPage::errorString() const
{
    return m_errorString;
}

QDateTime YellowPage::lastUpdatedTime()
{
    return m_lastUpdatedTime;
}

QUrl YellowPage::channelListUrl() const
{
    QUrl url = m_url;
    if (m_type == IndexTxt) {
        url = url.resolved(QUrl("./index.txt"));
        if (m_usePCRawProxy) {
            QUrl serverUrl = qApp->settings()->value("PCRaw/ProxyServerUrl").toString();
            QString path = url.toString();
            url.setHost(serverUrl.host());
            url.setPort(serverUrl.port());
            url.setPath(path);
        }
        QList<QPair<QString, QString> > query;
        QUrl pcUrl(qApp->settings()->value("PeerCast/ServerUrl").toString());
        query << QPair<QString, QString>("host", "localhost:" + QString::number(pcUrl.port()));
        if (hasNameSpace())
            query << QPair<QString, QString>("ns", m_nameSpaces.join("|"));
        url.setQueryItems(query);
    }
    return url;
}

ChannelList &YellowPage::channels()
{
    return m_channels;
}

ChannelList &YellowPage::stoppedChannels()
{
    return m_stoppedChannels;
}

YellowPageList &YellowPage::yellowPages()
{
    return m_yellowPages;
}


bool YellowPage::usePCRawProxy() const
{
    return m_usePCRawProxy;
}

void YellowPage::setUsePCRawProxy(bool enabled)
{
    m_usePCRawProxy = enabled;
}

bool YellowPage::isManager() const
{
    return m_type == Manager;
}

bool YellowPage::isEnabled() const
{
    return m_enabled;
}

void YellowPage::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

QString YellowPage::name() const
{
    return m_name;
}

void YellowPage::setName(const QString &name)
{
    m_name = name;
}

QStringList YellowPage::nameSpaces() const
{
    return m_nameSpaces;
}

bool YellowPage::hasNameSpace() const
{
    return !m_nameSpaces.isEmpty();
}

void YellowPage::setNameSpaces(const QStringList &nameSpaces)
{
    m_nameSpaces = nameSpaces;
}

QUrl YellowPage::url() const
{
    return m_url;
}

void YellowPage::setUrl(const QUrl &url)
{
    m_url = url;
}

YellowPage::Type YellowPage::type() const
{
    return m_type;
}

void YellowPage::setType(Type type)
{
    m_type = type;
}

QString YellowPage::typeString(Type type)
{
    switch (type) {
    case IndexTxt:
        return tr("index.txt");
    case Xml:
        return tr("XML(本家)");
    default:
        return QString();
    }
}

QDebug operator<<(QDebug dbg, const YellowPage &y)
{
    dbg << "YellowPage:"     << &y       << endl
        << "    name:"       << y.name() << endl
        << "    url:"        << y.url()  << endl
        << "    type:"       << y.type() << endl
        << "    typeString:" << YellowPage::typeString(y.type()) << endl;
    return dbg;
}

QDebug operator<<(QDebug dbg, YellowPage *y)
{ return dbg << *y; }

