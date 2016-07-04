/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <QtGui>

#include "ui_networkwidget.h"
#include "settingwidget.h"

class Settings;

class NetworkWidget : public SettingWidget, private Ui::NetworkWidget
{
    Q_OBJECT
public:
    NetworkWidget(Settings *settings, QWidget *parent = 0);
    virtual ~NetworkWidget();

    void write();
    void setValue(bool reset = false);

private slots:
    void on_selectPeerCastProgramButton_clicked();

private:
    Settings *m_settings;
};

#endif // NETWORKWIDGET_H
