/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "notificationwidget.h"
#include "settings.h"
#include "sound.h"

NotificationWidget::NotificationWidget(Settings *settings, QWidget *parent)
    : SettingWidget(parent), m_settings(settings)
{
    setupUi(this);
    setDirty();
    QCompleter *completer = new QCompleter(this);
    completer->setModel(new QDirModel(completer));
    soundFileEdit->setCompleter(completer);
}

NotificationWidget::~NotificationWidget()
{
}

void NotificationWidget::setValue(bool reset)
{
    Settings *settings = reset ? m_settings->defaultSettings() : m_settings;
    notifyFavoriteCheckBox->setChecked(settings->value("Notification/NotifyFavorite").toBool());
    minimumScoreSpinBox->setValue(settings->value("Notification/MinimumScore").toInt());
    playSoundCheckBox->setChecked(settings->value("Notification/PlaySound").toBool());
    soundFileEdit->setText(settings->value("Notification/SoundFile").toString());
}

void NotificationWidget::write()
{
    m_settings->setValue("Notification/NotifyFavorite", notifyFavoriteCheckBox->isChecked());
    m_settings->setValue("Notification/MinimumScore", minimumScoreSpinBox->value());
    m_settings->setValue("Notification/PlaySound", playSoundCheckBox->isChecked());
    m_settings->setValue("Notification/SoundFile", soundFileEdit->text());
}

void NotificationWidget::on_selectFileButton_clicked()
{
    QString dir;
    if (!soundFileEdit->text().isEmpty()) {
        dir = QFileInfo(soundFileEdit->text()).dir().canonicalPath();
    } else {
#ifdef Q_WS_X11
        dir = "/usr/share/sounds";
#endif
#ifdef Q_WS_WIN
        dir = "C:/WINDOWS/Media";
#endif
    }
    QString file = QFileDialog::getOpenFileName(this, tr("音声ファイルを選択"), dir, tr("音声ファイル (*.wav *.ogg *.mp3)"));
    if (!file.isEmpty())
        soundFileEdit->setText(file);
}

void NotificationWidget::on_playSoundButton_clicked()
{
    if (!soundFileEdit->text().isEmpty())
        Sound::play(soundFileEdit->text());
}

