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

class YellowPage;

class Channel : public QObject
{
    Q_OBJECT
public:
    enum StatusFlag {
        Stopped  = 0x0001000,
        Normal   = 0x0002000,
        Changed  = 0x0004000,
        New      = 0x0008000,
        Favorite = 0x0010000
    };
    Q_DECLARE_FLAGS(Status, StatusFlag)
    
    Channel(const QString &name, YellowPage *yellowPage = 0);
    Channel(YellowPage *yellowPage = 0);
    virtual ~Channel();

    YellowPage *yellowPage() const;
    void setYellowPage(YellowPage *yellowPage);

    bool isYPInfo() const;

    bool isPlayable() const;
    QUrl streamUrl(const QString &scheme = "http") const;
    QUrl playlistUrl() const;

    Status status() const;
    void setStatus(Status status);

    bool isFavorite() const;

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
    bool hasContactUrl() const;

    qint32 uptime() const;
    QString uptimeString() const;
    void setUptime(qint32 uptime);
    void setUptime(const QString &uptime);

    int listeners() const;
    void setListeners(int listeners);
    QString listenersString() const;

    int relays() const;
    void setRelays(int relays);
    QString relaysString() const;

    int score() const;
    bool hasScore() const;
    void setScore(int score);

    int bitrate() const;
    void setBitrate(int bitrate);
    QString bitrateString() const;

protected:
    YellowPage *m_yellowPage;

private:
    void init();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Channel::Status)
typedef QList<Channel *> ChannelList;

QDebug operator<<(QDebug dbg, const Channel &c);
QDebug operator<<(QDebug dbg, Channel *c);

#endif // CHANNEL_H
