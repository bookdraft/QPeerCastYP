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
#include "utils.h"

PlayerEdit::PlayerEdit(Settings *settings, QWidget *parent)
    : SettingWidget(parent), m_settings(settings)
{
    setupUi(this);
    setDirty();
    QCompleter *completer = new QCompleter(this);
    completer->setModel(new QDirModel(completer));
    videoPlayerEdit->setCompleter(completer);
    soundPlayerEdit->setCompleter(completer);
    label->setText(tr("<table cellspacing=6 width=100%>"
                      "<tr><td>$STREAM_URL</td><td>ストリーム URL (HTTP)</td></tr>"
                      "<tr><td>$STREAM_URL(scheme)</td><td>ストリーム URL (スキームを指定)</td></tr>"
                      "<tr><td>$CHANNEL(property)</td><td>チャンネル情報</td></tr></table>"));
}

PlayerEdit::~PlayerEdit()
{
}

void PlayerEdit::setValue(bool reset)
{
    Settings *s = reset ? m_settings->defaultSettings() : m_settings;
    if (reset) {
        QString ws = Utils::wsString();
        s->beginReadArray(QString("Player%1/Items").arg(ws));
    } else {
        s->beginReadArray("Player/Items");
    }

    s->setArrayIndex(0);
    videoPlayerEdit->setText(s->value("Program").toString());
    videoPlayerArgsEdit->setText(s->value("Args").toString());
    videoTypesEdit->setText(s->value("Types").toString());

    s->setArrayIndex(1);
    soundPlayerEdit->setText(s->value("Program").toString());
    soundPlayerArgsEdit->setText(s->value("Args").toString());
    soundTypesEdit->setText(s->value("Types").toString());

    s->endArray();
}

void PlayerEdit::write()
{
    m_settings->beginWriteArray("Player/Items");
    m_settings->setArrayIndex(0);
    m_settings->setValue("Program", videoPlayerEdit->text());
    m_settings->setValue("Args", videoPlayerArgsEdit->text());
    m_settings->setValue("Types", videoTypesEdit->text());
    m_settings->setArrayIndex(1);
    m_settings->setValue("Program", soundPlayerEdit->text());
    m_settings->setValue("Args", soundPlayerArgsEdit->text());
    m_settings->setValue("Types", soundTypesEdit->text());
    m_settings->endArray();
}

void PlayerEdit::on_selectVideoPlayerButton_clicked()
{
    QString program = getProgram(videoPlayerEdit->text());
    if (!program.isEmpty()) {
        videoPlayerEdit->setText(program);
        if (program.endsWith("pcwmp.exe"))
            videoPlayerArgsEdit->setText("\"$STREAM_URL\" \"$CHANNEL(name)\"");
    }
}

void PlayerEdit::on_selectSoundPlayerButton_clicked()
{
    QString program = getProgram(soundPlayerEdit->text());
    if (!program.isEmpty()) {
        soundPlayerEdit->setText(program);
        if (program.endsWith("pcwmp.exe"))
            soundPlayerArgsEdit->setText("\"$STREAM_URL\" \"$CHANNEL(name)\"");
    }
}

QString PlayerEdit::getProgram(const QString &currentProgram)
{
    QString dir = ".";
    if (!currentProgram.isEmpty()) {
        dir = QFileInfo(currentProgram).dir().absolutePath();
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

void PlayerEdit::on_label_linkActivated(const QString &link)
{
    Q_UNUSED(link);
}

