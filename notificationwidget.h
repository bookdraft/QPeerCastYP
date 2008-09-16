/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QtGui>

#include "settingwidget.h"
#include "ui_notificationwidget.h"

class Settings;

class NotificationWidget : public SettingWidget, private Ui::NotificationWidget
{
    Q_OBJECT
public:
    NotificationWidget(Settings *settings, QWidget *parent = 0);
    virtual ~NotificationWidget();

    void setValue(bool reset = false);
    void write();

private slots:
    void notifyChannelCheckBoxChanged();
    void on_selectFileButton_clicked();
    void on_playSoundButton_clicked();

private:
    Settings *m_settings;
};

#endif // NOTIFICATIONWIDGET_H
