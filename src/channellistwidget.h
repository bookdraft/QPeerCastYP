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
    enum Column { Status, Name, Description, Listeners, Relays, Score, Uptime, Bitrate, Type, LabelCount };
    enum LinkType { ChannelLink, ContactLink };

    ChannelListWidget(QWidget *parent = 0, YellowPage *yellowPage = 0);
    virtual ~ChannelListWidget();

    YellowPage *yellowPage() const;
    void setYellowPage(YellowPage *yp);

    bool customToolTip() const;
    void setCustomToolTip(bool enable);

    LinkType linkType() const;
    void setLinkType(int type);

    bool linkEnabled() const;
    void setLinkEnabled(bool enable);

    bool linkUnderline() const;
    void setLinkUnderline(bool enable);

    bool linkBold() const;
    void setLinkBold(bool enable);

    QColor linkColor() const;
    void setLinkColor(const QColor &c);

    QString linkStatusTipFormat(LinkType type) const;
    void setLinkStatusTipFormat(LinkType type, const QString &format);

    QList<int> sortOrder() const;
    void setSortOrder(const QList<int> &order);

    int minimumItemHeight() const;
    void setMinimumItemHeight(int height);

    int channelCount() const;
    int listenerCount() const;
    int yellowPageCount() const;

    QDateTime lastUpdatedTime();
    Channel *currentChannel() const;
    bool isActive() const;
    void setActive(bool active);
    void updateActions();
    void readSettings();

public slots:
    void updateYellowPage();
    void updateItems();
    void updateLinks();
    void currentItemChanged(QTreeWidgetItem *current);
    void findItems(const QString &text, Qt::MatchFlags flags = Qt::MatchContains);
    void playChannel(Channel *channel = 0);
    void addToFavorites();
    void openContactUrl();
    void copyStreamUrl();
    void copyContactUrl();
    void copyChannelInfo();

signals:
    void linkHovered(Channel *channel);
    void linkClicked(Channel *channel);

protected:
    void polish();
    QRect linkTextRect(QTreeWidgetItem *item, Column column) const;
    void addItems(const QList<Channel *> &channels);
    void updateCursor();

    bool event(QEvent *event);
    bool viewportEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void done(YellowPage *yp, bool error);
    void onLinkHovered(Channel *channel);
    void onLinkClicked(Channel *channel);
    void headerContextMenuRequested(const QPoint &pos);

private:
    YellowPage *m_yellowPage;
    QDateTime m_lastUpdatedTime;
    QList<Channel *> m_favoriteChannels;
    bool m_active;
    bool m_customToolTip;
    LinkType m_linkType;
    QMap<int, QString> m_linkStatusTipFormats;
    bool m_linkEnabled;
    bool m_linkUnderline;
    bool m_linkBold;
    bool m_linkHovered;
    QColor m_linkColor;
    Channel *m_pressedChannel;
    Channel *m_lastHoveredLink;
    int m_minimumItemHeight;
    bool m_needClear;
    QList<int> m_sortOrder;
};

#endif // CHANNELLISTWIDGET_H
