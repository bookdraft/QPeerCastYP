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

Channel::Channel(const QString &name)
    : m_status(New), m_name(name), m_uptime(0),
      m_listeners(-1), m_relays(-1), m_score(0), m_bitrate(-1)
{
}

Channel::~Channel()
{
}

bool Channel::isPlayable() const
{
    return m_id != "00000000000000000000000000000000";
}

QUrl Channel::streamUrl(const QString &scheme) const
{
    QUrl url = qApp->settings()->value("PeerCast/ServerUrl").toString();
    return QUrl(QString("%1://%2:%3/stream/%4.%5?tip=%6")
                        .arg(scheme).arg(url.host()).arg(url.port())
                        .arg(m_id).arg(m_type.toLower()).arg(m_tip));
}

Channel::Status Channel::status() const
{
    return m_status;
}

void Channel::setStatus(Status status)
{
    m_status = status;
}

QString Channel::longDescription() const
{
    QString desc;
    QStringList list;
    if (!m_genre.isEmpty())
        desc += "[" + m_genre + "] ";
    list << m_description << m_artist << m_title << m_album << m_message;
    list.removeAll(QString());
    desc += list.join(" - ");

    return desc;
}

QString Channel::name(bool removeStatus) const
{
    QString name = m_name;
    if (removeStatus)
        name.remove(QRegExp("( \\((切断|再生不可|要帯域チェック|要ポート開放)[^)]*\\))*$"));
    return name;
}

void Channel::setName(const QString &name)
{
    m_name = name;
}

QString Channel::id() const
{
    return m_id;
}

void Channel::setId(const QString &id)
{
    m_id = id;
}

QString Channel::tip() const
{
    return m_tip;
}

void Channel::setTip(const QString &tip)
{
    m_tip = tip;
}

QString Channel::genre() const
{
    return m_genre;
}

void Channel::setGenre(const QString &genre)
{
    m_genre = genre;
}

QString Channel::description() const
{
    return m_description;
}

void Channel::setDescription(const QString &description)
{
    m_description = description;
}

QString Channel::artist() const
{
    return m_artist;
}

void Channel::setArtist(const QString &artist)
{
    m_artist = artist;
}

QString Channel::album() const
{
    return m_album;
}

void Channel::setAlbum(const QString &album)
{
    m_album = album;
}

QString Channel::title() const
{
    return m_title;
}

void Channel::setTitle(const QString &title)
{
    m_title = title;
}

QString Channel::message() const
{
    return m_message;
}

void Channel::setMessage(const QString &message)
{
    m_message = message;
}

QString Channel::type() const
{
    return m_type;
}

void Channel::setType(const QString &type)
{
    m_type = type.toUpper();
}

QUrl Channel::contactUrl() const
{
    return m_contactUrl;
}

void Channel::setContactUrl(const QString &url)
{
    setContactUrl(QUrl(url));
}

void Channel::setContactUrl(const QUrl &url)
{
    m_contactUrl = url;
}

QUrl Channel::url() const
{
    return m_url;
}

void Channel::setUrl(const QUrl &url)
{
    m_url = url;
}

// 配信時間(単位は秒)
qint32 Channel::uptime() const
{
    return m_uptime;
}

void Channel::setUptime(qint32 uptime)
{
    m_uptime = uptime;
}

void Channel::setUptime(const QString &uptime) {
    int uptime_ = 0;
    QStringList t = uptime.split(":");
    uptime_ += t[0].toInt() * 60 * 60;
    uptime_ += t[1].toInt() * 60;
    m_uptime = uptime_; 
}

QString Channel::uptimeString() const {
    int days  = m_uptime / (24 * 60 * 60);
    int hours = (m_uptime - days * 24 * 60 * 60) / (60 * 60);
    int mins  = (m_uptime - days * 24 * 60 * 60 - hours * 60 * 60) / 60;
    QString str = days ? QString("%1:").arg(days) : "";
    return str + QString().sprintf("%02d:%02d", hours, mins);
}

int Channel::listeners() const
{
    return m_listeners;
}

void Channel::setListeners(int listeners)
{
    m_listeners = listeners;
}

int Channel::relays() const
{
    return m_relays;
}

void Channel::setRelays(int relays)
{
    m_relays = relays;
}

int Channel::score() const
{
    return m_score;
}

bool Channel::hasScore() const
{
    return m_score > 0;
}

void Channel::setScore(int score)
{
    m_score = score;
}

int Channel::bitrate() const
{
    return m_bitrate;
}

void Channel::setBitrate(int bitrate)
{
    m_bitrate = bitrate;
}

QDebug operator<<(QDebug dbg, const Channel &c)
{
    dbg << "Channel:"          << &c                 << endl
        << "    name:"         << c.name()           << endl
        << "    id:"           << c.id()             << endl
        << "    tip:"          << c.tip()            << endl
        << "    genre:"        << c.genre()          << endl
        << "    description:"  << c.description()    << endl
        << "    artist:"       << c.artist()         << endl
        << "    title:"        << c.title()          << endl
        << "    message:"      << c.message()        << endl
        << "    url:"          << c.url().toString() << endl
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
    return dbg << *c;
}

