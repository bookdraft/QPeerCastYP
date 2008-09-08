/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef ADVANCEDWIDGET_H
#define ADVANCEDWIDGET_H

#include "ui_advancedwidget.h"
#include "settingwidget.h"

class Settings;

class AdvancedWidget : public SettingWidget, private Ui::AdvancedWidget
{
public:
    AdvancedWidget(Settings *settings, QWidget *parent);
    virtual ~AdvancedWidget();

    void setValue(bool reset = false);
    void write();

private:
    Settings *m_settings;
};

#endif // ADVANCEDWIDGET_H
