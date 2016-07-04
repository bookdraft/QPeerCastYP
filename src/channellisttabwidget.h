/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef CHANNELLISTTABWIDGET_H
#define CHANNELLISTTABWIDGET_H

#include <QtGui>

class ChannelListWidget;

class ChannelListTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    ChannelListTabWidget(QWidget *parent = 0);
    virtual ~ChannelListTabWidget();

    ChannelListWidget *currentWidget();
    ChannelListWidget *widget(int index);

    void setActive(bool active);
    void setTabBarVisible(bool shown);
    bool tabBarIsVisible() const;

private slots:
    void findRequest(QString text, Qt::MatchFlags flags);
    void currentChanged(int index);
};

#endif // CHANNELLISTTABWIDGET_H
