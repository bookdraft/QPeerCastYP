/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef CHANNEL_H
#define CHANNEL_H

#include <QtCore>

class Channel
{
public:
    enum Status { Stopped, Normal, Changed,  New };

    Channel(const QString &name = QString());
    virtual ~Channel();

    bool isPlayable() const;
    QUrl streamUrl(const QString &scheme = "http") const;

    Status status() const;
    void setStatus(Status status);

    QString name(bool removeStatus = false) const;
    void setName(const QString &name);

    QString id() const;
    void setId(const QString &id);

    QString tip() const;
    void setTip(const QString &tip);

    QString genre() const;
    void setGenre(const QString &genre);

    QString description() const;
    QString longDescription() const;
    void setDescription(const QString &description);

    QString artist() const;
    void setArtist(const QString &artist);

    QString album() const;
    void setAlbum(const QString &album);

    QString title() const;
    void setTitle(const QString &title);

    QString message() const;
    void setMessage(const QString &message);

    QString type() const;
    void setType(const QString &type);

    QUrl contactUrl() const;
    void setContactUrl(const QString &url);
    void setContactUrl(const QUrl &url);

    QUrl url() const;
    void setUrl(const QUrl &url);

    qint32 uptime() const;
    QString uptimeString() const;
    void setUptime(qint32 uptime);
    void setUptime(const QString &uptime);

    int listeners() const;
    void setListeners(int listeners);

    int relays() const;
    void setRelays(int relays);

    int score() const;
    bool hasScore() const;
    void setScore(int score);

    int bitrate() const;
    void setBitrate(int bitrate);

private:
    Status m_status;
    QString m_name;
    QString m_id;
    QString m_tip;
    QString m_genre;
    QString m_description;
    QString m_artist;
    QString m_album;
    QString m_title;
    QString m_message;
    QString m_type;
    QUrl m_contactUrl;
    QUrl m_url;
    qint32 m_uptime;
    int m_listeners;
    int m_relays;
    int m_score;
    int m_bitrate;
};

typedef QList<Channel *> ChannelList;

QDebug operator<<(QDebug dbg, const Channel &c);
QDebug operator<<(QDebug dbg, Channel *c);

#endif // CHANNEL_H
