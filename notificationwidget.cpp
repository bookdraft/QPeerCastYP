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
    connect(notifyFavoriteCheckBox, SIGNAL(toggled(bool)), SLOT(notifyChannelCheckBoxChanged()));
    connect(notifyNewCheckBox, SIGNAL(toggled(bool)), SLOT(notifyChannelCheckBoxChanged()));
    connect(notifyChangedCheckBox, SIGNAL(toggled(bool)), SLOT(notifyChannelCheckBoxChanged()));
}

NotificationWidget::~NotificationWidget()
{
}

void NotificationWidget::notifyChannelCheckBoxChanged()
{
    bool enabled = notifyFavoriteCheckBox->isChecked()
                    or notifyNewCheckBox->isChecked()
                    or notifyChangedCheckBox->isChecked();
    favoriteGroupBox->setEnabled(enabled);
    balloonGroupBox->setEnabled(enabled);
    soundGroupBox->setEnabled(enabled);
}

void NotificationWidget::setValue(bool reset)
{
    Settings *settings = reset ? m_settings->defaultSettings() : m_settings;
    notifyFavoriteCheckBox->setChecked(settings->value("Notification/NotifyFavorite").toBool());
    notifyNewCheckBox->setChecked(settings->value("Notification/NotifyNew").toBool());
    notifyChangedCheckBox->setChecked(settings->value("Notification/NotifyChanged").toBool());
    notifyChangedFavoriteCheckBox->setChecked(settings->value("Notification/NotifyChangedFavorite").toBool());
    minimumScoreSpinBox->setValue(settings->value("Notification/MinimumScore").toInt());
    showBalloonMessageCheckBox->setChecked(settings->value("Notification/ShowBalloonMessage").toBool());
    maxShowChannelsSpinBox->setValue(settings->value("Notification/MaximumShowChannels").toInt());
    playSoundCheckBox->setChecked(settings->value("Notification/PlaySound").toBool());
    soundFileEdit->setText(settings->value("Notification/SoundFile").toString());
}

void NotificationWidget::write()
{
    m_settings->setValue("Notification/NotifyFavorite", notifyFavoriteCheckBox->isChecked());
    m_settings->setValue("Notification/NotifyNew", notifyNewCheckBox->isChecked());
    m_settings->setValue("Notification/NotifyChanged", notifyChangedCheckBox->isChecked());
    m_settings->setValue("Notification/NotifyChangedFavorite", notifyChangedFavoriteCheckBox->isChecked());
    m_settings->setValue("Notification/MinimumScore", minimumScoreSpinBox->value());
    m_settings->setValue("Notification/ShowBalloonMessage", showBalloonMessageCheckBox->isChecked());
    m_settings->setValue("Notification/MaximumShowChannels", maxShowChannelsSpinBox->value());
    m_settings->setValue("Notification/PlaySound", playSoundCheckBox->isChecked());
    m_settings->setValue("Notification/SoundFile", soundFileEdit->text());
}

void NotificationWidget::on_selectFileButton_clicked()
{
    QString dir;
    if (!soundFileEdit->text().isEmpty()) {
        dir = QFileInfo(soundFileEdit->text()).dir().canonicalPath();
    } else {
#ifdef Q_WS_MAC
        dir = "/System/Library/Sounds";
#endif
#ifdef Q_WS_X11
        dir = "/usr/share/sounds";
#endif
#ifdef Q_WS_WIN
        dir = "C:/WINDOWS/Media";
#endif
    }
    QString file = QFileDialog::getOpenFileName(this, tr("音声ファイルを選択"), dir, tr("音声ファイル (*.wav *.aiff *.ogg *.mp3)"));
    if (!file.isEmpty())
        soundFileEdit->setText(file);
}

void NotificationWidget::on_playSoundButton_clicked()
{
    if (!soundFileEdit->text().isEmpty())
        Sound::play(soundFileEdit->text());
}

