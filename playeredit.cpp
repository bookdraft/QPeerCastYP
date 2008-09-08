/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "playeredit.h"
#include "settings.h"

PlayerEdit::PlayerEdit(Settings *settings, QWidget *parent)
    : SettingWidget(parent), m_settings(settings)
{
    setupUi(this);
    setDirty(true);
    QCompleter *completer = new QCompleter(this);
    completer->setModel(new QDirModel(completer));
    videoPlayerEdit->setCompleter(completer);
    soundPlayerEdit->setCompleter(completer);
    label->setText(tr("<table cellspacing=6 width=100%>"
                   "<tr><td>$STREAM_URL</td><td>ストリーム URL (http)</td></tr>"
                   "<tr><td>$STREAM_URL(scheme)</td><td>ストリーム URL (mms, mmsh などのスキームを指定)</td></tr>"
                   "<tr><td>$CHANNEL_NAME</td><td>チャンネル名</td></tr></table>"));
}

PlayerEdit::~PlayerEdit()
{
}

void PlayerEdit::setValue(bool reset)
{
    Settings *settings = reset ? m_settings->defaultSettings() : m_settings;
    if (reset) {
#ifdef Q_WS_X11
        videoPlayerEdit->setText(settings->value("Player/Linux/VideoPlayer").toString());
        videoPlayerArgsEdit->setText(settings->value("Player/Linux/VideoPlayerArgs").toString());
        soundPlayerEdit->setText(settings->value("Player/Linux/SoundPlayer").toString());
        soundPlayerArgsEdit->setText(settings->value("Player/Linux/SoundPlayerArgs").toString());
#endif
#ifdef Q_WS_WIN
        videoPlayerEdit->setText(settings->value("Player/Windows/VideoPlayer").toString());
        videoPlayerArgsEdit->setText(settings->value("Player/Windows/VideoPlayerArgs").toString());
        soundPlayerEdit->setText(settings->value("Player/Windows/SoundPlayer").toString());
        soundPlayerArgsEdit->setText(settings->value("Player/Windows/SoundPlayerArgs").toString());
#endif
    } else {
        videoPlayerEdit->setText(settings->value("Player/VideoPlayer").toString());
        videoPlayerArgsEdit->setText(settings->value("Player/VideoPlayerArgs").toString());
        soundPlayerEdit->setText(settings->value("Player/SoundPlayer").toString());
        soundPlayerArgsEdit->setText(settings->value("Player/SoundPlayerArgs").toString());
    }
    videoTypesEdit->setText(settings->value("Player/VideoTypes").toString());
    soundTypesEdit->setText(settings->value("Player/SoundTypes").toString());
}

void PlayerEdit::write()
{
    m_settings->setValue("Player/VideoPlayer", videoPlayerEdit->text());
    m_settings->setValue("Player/VideoPlayerArgs", videoPlayerArgsEdit->text());
    m_settings->setValue("Player/VideoTypes", videoTypesEdit->text());
    m_settings->setValue("Player/SoundPlayer", soundPlayerEdit->text());
    m_settings->setValue("Player/SoundPlayerArgs", soundPlayerArgsEdit->text());
    m_settings->setValue("Player/SoundTypes", soundTypesEdit->text());
}

void PlayerEdit::on_selectVideoPlayerButton_clicked()
{
    QString program = getProgram(videoPlayerEdit->text());
    if (!program.isEmpty()) {
        videoPlayerEdit->setText(program);
        if (program.endsWith("pcwmp.exe"))
            videoPlayerArgsEdit->setText("\"$STREAM_URL\" \"$CHANNEL_NAME\"");
    }
}

void PlayerEdit::on_selectSoundPlayerButton_clicked()
{
    QString program = getProgram(soundPlayerEdit->text());
    if (!program.isEmpty()) {
        soundPlayerEdit->setText(program);
        if (program.endsWith("pcwmp.exe"))
            videoPlayerArgsEdit->setText("\"$STREAM_URL\" \"$CHANNEL_NAME\"");
    }
}

QString PlayerEdit::getProgram(const QString &currentProgram)
{
    QString dir = ".";
    if (!currentProgram.isEmpty()) {
        dir = QFileInfo(currentProgram).dir().canonicalPath();
    } else {
#ifdef Q_WS_X11
        dir = "/usr/bin";
#endif
#ifdef Q_WS_WIN
        dir = "C:/Program Files";
#endif
    }
    return QFileDialog::getOpenFileName(this, tr("プログラムを選択"), dir);
}

