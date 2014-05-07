/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "channel.h"
#include "application.h"
#include "settings.h"
#include "yellowpage.h"

Channel::Channel(const QString &name, YellowPage *yellowPage)
    : QObject(yellowPage), m_yellowPage(yellowPage)
{
    setProperty("name", name);
    init();
}

Channel::Channel(YellowPage *yellowPage)
    : QObject(yellowPage), m_yellowPage(yellowPage)
{
    init();
}

Channel::~Channel()
{
}

void Channel::init()
{
    setProperty("status", New);
    setProperty("uptime", 0);
    setProperty("listeners", -1);
    setProperty("relays", -1);
    setProperty("score", 0);
    setProperty("bitrate", -1);
}

YellowPage *Channel::yellowPage() const
{
    return m_yellowPage;
}

void Channel::setYellowPage(YellowPage *yellowPage)
{
    m_yellowPage = yellowPage;
}

bool Channel::isYPInfo() const
{
    return type().toUpper() == "RAW" or name().indexOf(QRegExp("^[\\w\\s]+◆")) != -1;
}

bool Channel::isPlayable() const
{
    return id() != "00000000000000000000000000000000";
}

QUrl Channel::streamUrl(const QString &scheme) const
{
    QUrl url = qApp->settings()->value("PeerCast/ServerUrl").toString();
    return QUrl(QString("%1://%2:%3/stream/%4.%5?tip=%6")
                        .arg(scheme).arg(url.host()).arg(url.port())
                        .arg(id()).arg(type().toLower()).arg(tip()));
}

QUrl Channel::playlistUrl() const
{
    QUrl url = qApp->settings()->value("PeerCast/ServerUrl").toString();
    return QUrl(QString("http://%1:%2/pls/%3.pls?tip=%5")
                        .arg(url.host()).arg(url.port()).arg(id()).arg(tip()));
}

Channel::Status Channel::status() const
{
    return (Status)property("status").value<qint32>();
}

void Channel::setStatus(Status status)
{
    setProperty("status", (qint32)status);
}

bool Channel::isFavorite() const
{
    return status() & Favorite;
}

QString Channel::longDescription() const
{
    QString desc;
    QStringList list;
    if (!genre().isEmpty())
        desc += "[" + genre() + "] ";
    list << description() << artist() << title() << album() << message();
    list.removeAll(QString());
    desc += list.join(" - ");

    return desc;
}

QString Channel::name(bool removeStatus) const
{
    QString name = property("name").toString();
    if (removeStatus)
        name.remove(QRegExp("( \\((切断|再生不可|要帯域チェック|要ポート開放)[^)]*\\))*$"));
    return name;
}

void Channel::setName(const QString &name)
{
    setProperty("name", name);
}

QString Channel::id() const
{
    return property("id").toString();
}

void Channel::setId(const QString &id)
{
    setProperty("id", id);
}

QString Channel::tip() const
{
    return property("tip").toString();
}

void Channel::setTip(const QString &tip)
{
    setProperty("tip", tip);
}

QString Channel::genre() const
{
    return property("genre").toString();
}

void Channel::setGenre(const QString &genre)
{
    setProperty("genre", genre);
}

QString Channel::description() const
{
    return property("description").toString();
}

void Channel::setDescription(const QString &description)
{
    setProperty("description", description);
}

QString Channel::artist() const
{
    return property("artist").toString();
}

void Channel::setArtist(const QString &artist)
{
    setProperty("artist", artist);
}

QString Channel::album() const
{
    return property("album").toString();
}

void Channel::setAlbum(const QString &album)
{
    setProperty("album", album);
}

QString Channel::title() const
{
    return property("title").toString();
}

void Channel::setTitle(const QString &title)
{
    setProperty("title", title);
}

QString Channel::message() const
{
    return property("message").toString();
}

void Channel::setMessage(const QString &message)
{
    setProperty("message", message);
}

QString Channel::type() const
{
    return property("type").toString();
}

void Channel::setType(const QString &type)
{
    setProperty("type", type.toUpper());
}

QUrl Channel::contactUrl() const
{
    return QUrl(property("contact_url").toString());
}

void Channel::setContactUrl(const QString &url)
{
    setProperty("contact_url", url);
}

void Channel::setContactUrl(const QUrl &url)
{
    setContactUrl(url.toString());
}

bool Channel::hasContactUrl() const
{
    return !contactUrl().isEmpty();
}

// 配信時間(単位は秒)
qint32 Channel::uptime() const
{
    return property("uptime").value<qint32>();
}

void Channel::setUptime(qint32 uptime)
{
    setProperty("uptime", uptime);
    setProperty("uptime_string", uptimeString());
}

void Channel::setUptime(const QString &uptime) {
    qint32 secs = 0;
    QStringList t = uptime.split(":");
    if (t.size() == 2) {
      secs += t[0].toInt() * 60 * 60;
      secs += t[1].toInt() * 60;
    }
    setUptime(secs); 
}

QString Channel::uptimeString() const {
    int days  = uptime() / (24 * 60 * 60);
    int hours = (uptime() - days * 24 * 60 * 60) / (60 * 60);
    int mins  = (uptime() - days * 24 * 60 * 60 - hours * 60 * 60) / 60;
    QString str = days ? QString("%1:").arg(days) : "";
    return str + QString().sprintf("%02d:%02d", hours, mins);
}

int Channel::listeners() const
{
    return property("listeners").toInt();
}

void Channel::setListeners(int listeners)
{
    setProperty("listeners", listeners);
    setProperty("listeners_string", listenersString());
}

QString Channel::listenersString() const
{
    return listeners() < 0 ? "-" : QString::number(listeners());
}

int Channel::relays() const
{
    return property("relays").toInt();
}

void Channel::setRelays(int relays)
{
    setProperty("relays", relays);
    setProperty("relays_string", relaysString());
}

QString Channel::relaysString() const
{
    return relays() < 0 ? "-" : QString::number(relays());
}

int Channel::score() const
{
    return property("score").toInt();
}

bool Channel::hasScore() const
{
    return score() > 0;
}

void Channel::setScore(int score)
{
    setProperty("score", score);
}

int Channel::bitrate() const
{
    return property("bitrate").toInt();
}

void Channel::setBitrate(int bitrate)
{
    setProperty("bitrate", bitrate);
}

QString Channel::bitrateString() const
{
    return QString::number(bitrate()) + "kb/s";
}

QDebug operator<<(QDebug dbg, const Channel &c)
{
    dbg << &c << endl
        << "    name:"         << c.name()           << endl
        << "    id:"           << c.id()             << endl
        << "    tip:"          << c.tip()            << endl
        << "    genre:"        << c.genre()          << endl
        << "    description:"  << c.description()    << endl
        << "    artist:"       << c.artist()         << endl
        << "    title:"        << c.title()          << endl
        << "    message:"      << c.message()        << endl
        << "    uptime:"       << c.uptime()         << endl
        << "    uptimeString:" << c.uptimeString()   << endl
        << "    listeners:"    << c.listeners()      << endl
        << "    relays:"       << c.relays()         << endl
        << "    score:"        << c.score()          << endl
        << "    bitrate:"      << c.bitrate()        << endl;

    return dbg;
}

QDebug operator<<(QDebug dbg, Channel *c)
{
    if (c)
        return dbg << *c;
    else
        return dbg << "Null";
}

