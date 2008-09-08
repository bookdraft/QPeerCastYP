/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef CHANNELLISTFINDBAR_H
#define CHANNELLISTFINDBAR_H

#include <QtGui>

#include "ui_channellistfindbar.h"

class ChannelListFindBarEventFilter;

class ChannelListFindBar : public QWidget, private Ui::ChannelListFindBar
{
    Q_OBJECT
    friend class ChannelListFindBarEventFilter;
public:
    ChannelListFindBar(QWidget *parent = 0);
    virtual ~ChannelListFindBar();

    void show();

signals:
    void findRequest(const QString &text, Qt::MatchFlags flags);

private slots:
    void on_closeButton_clicked();
    void on_caseSensitivityCheckBox_stateChanged();
    void on_regExpCheckBox_stateChanged();
    void on_lineEdit_returnPressed();
    void on_lineEdit_textChanged();

private:
    void emitFindRequest();
};

class ChannelListFindBarEventFilter : public QObject
{
    Q_OBJECT
public:
    ChannelListFindBarEventFilter(ChannelListFindBar *parent)
        : QObject(parent), findBar(parent)
    {
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    ChannelListFindBar *findBar;
};


#endif // CHANNELLISTFINDBAR_H
