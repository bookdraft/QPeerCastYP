/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef GENERALWIDGET_H
#define GENERALWIDGET_H

#include "ui_generalwidget.h"
#include "settingwidget.h"

class Settings;

class GeneralWidget : public SettingWidget, private Ui::GeneralWidget
{
    Q_OBJECT
public:
    GeneralWidget(Settings *settings, QWidget *parent = 0);
    virtual ~GeneralWidget();

    void write();
    void setValue(bool reset = false);

private slots:
    void on_selectWebBrowserButton_clicked();

private:
    Settings *m_settings;
};

#endif // GENERALWIDGET_H
