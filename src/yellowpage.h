/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef YELLOWPAGE_H
#define YELLOWPAGE_H

#include <QtCore>
#include <QHttp>

#include "channel.h"

class YellowPage : public QObject
{
    Q_OBJECT
public:
    enum Type { IndexTxt, Xml };

    YellowPage(const QUrl &url, Type = IndexTxt);
    YellowPage(const QString &url = QString(), Type = IndexTxt);
    virtual ~YellowPage();

    virtual void update();
    virtual void stopUpdate();
    virtual bool isUpdating();

    QString errorString() const;

    virtual QDateTime lastUpdatedTime();

    QUrl channelListUrl() const;
    virtual ChannelList channels() const;
    virtual ChannelList channels(Channel::Status status) const;
    virtual QList<YellowPage *> &yellowPages();

    bool isManager() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    QString name() const;
    void setName(const QString &name);

    QStringList nameSpaces() const;
    bool hasNameSpace() const;
    void setNameSpaces(const QStringList &nameSpaces);

    QUrl url() const;
    void setUrl(const QUrl &url);

    Type type() const;
    void setType(Type type);

    static QString typeString(Type type);

signals:
    void updateDone(YellowPage *yellowPage, bool error);
    void channelListChanged();

protected slots:
    void timeout();
    virtual void done(bool error);

protected:
    bool m_isManager;
    ChannelList m_channels;
    QDateTime m_lastUpdatedTime;
    QList<YellowPage *> m_yellowPages;

private:
    void initialize();

    bool m_enabled;
    QString m_name;
    QStringList m_nameSpaces;
    QUrl m_url;
    Type m_type;
    QHttp *m_http;
    QTimer m_httpTimer;
    bool m_updating;
    QString m_errorString;
};

typedef QList<YellowPage *> YellowPageList;

QDebug operator<<(QDebug dbg, const YellowPage &y);
QDebug operator<<(QDebug dbg, YellowPage *y);

#endif // YELLOWPAGE_H
