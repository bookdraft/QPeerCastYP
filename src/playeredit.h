/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef PLAYEREDIT_H
#define PLAYEREDIT_H

#include <QtGui>

#include "ui_playeredit.h"
#include "settingwidget.h"

class Settings;

class PlayerEdit : public SettingWidget, private Ui::PlayerEdit
{
    Q_OBJECT
public:
    PlayerEdit(Settings *settings, QWidget *parent = 0);
    virtual ~PlayerEdit();

    void setValue(bool reset = false);
    void write();

private slots:
    void on_label_linkActivated(const QString &link);
    void on_selectVideoPlayerButton_clicked();
    void on_selectSoundPlayerButton_clicked();

private:
    QString getProgram(const QString &currentProgram);

    Settings *m_settings;
};

#endif // PLAYEREDIT_H
