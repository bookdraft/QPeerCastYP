/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef CHANNELLISTWIDGET_H
#define CHANNELLISTWIDGET_H

#include <QtGui>

class Channel;
class YellowPage;

class ChannelListWidget : public QTreeWidget
{
    Q_OBJECT
public:
    enum Column { Status, Name, Description, Listeners, Relays, Score, Uptime, Bitrate, Type, Headers };

    ChannelListWidget(QWidget *parent = 0, YellowPage *yellowPage = 0);
    virtual ~ChannelListWidget();

    YellowPage *yellowPage() const;
    void setYellowPage(YellowPage *yp);

    int channelCount() const;
    int listenerCount() const;
    int yellowPageCount() const;

    QDateTime lastUpdatedTime();
    Channel *currentChannel() const;
    bool isActive() const;
    void setActive(bool active);
    void updateActions();

public slots:
    void updateYellowPage();
    void updateItems();
    void currentItemChanged(QTreeWidgetItem *current);
    void filterItems(const QString &text, Qt::MatchFlags flags = Qt::MatchContains);
    void playChannel(Channel *channel = 0);
    void addToFavorites();
    void openContactUrl();
    void copyStreamUrl();
    void copyContactUrl();
    void copyChannelInfo();

protected:
    bool event(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void done(YellowPage *yp, bool error);
    void headerContextMenuRequested(const QPoint &pos);

private:
    void polish();
    void addItems(QList<Channel *> &channels);
    void notifyFavorite();

    YellowPage *m_yellowPage;
    QDateTime m_lastUpdatedTime;
    QList<Channel *> m_favoriteChannels;
    bool m_needClear;
    bool m_active;
};

#endif // CHANNELLISTWIDGET_H
