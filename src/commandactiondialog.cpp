/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "commandactiondialog.h"
#include "commandaction.h"

CommandActionDialog::CommandActionDialog(CommandAction *action, QWidget *parent)
    : QDialog(parent), m_action(action)
{
    setupUi(this);

    QCompleter *completer = new QCompleter(this);
    completer->setModel(new QDirModel(completer));
    programEdit->setCompleter(completer);

    if (!action->icon().isNull()) {
        m_iconFileName = action->iconFileName();
        iconButton->setIcon(action->icon());
    } else {
        iconButton->setText(tr("..."));
    }
    textEdit->setText(action->text());
    shortcutEdit->setText(action->shortcut().toString());
    programEdit->setText(action->program());
    argsEdit->setText(action->arguments());

    resize(minimumSizeHint().width() + 200, minimumSizeHint().height());
}

CommandActionDialog::~CommandActionDialog()
{
}

void CommandActionDialog::on_iconButton_clicked()
{
    QString dir;
#ifdef Q_WS_X11
    dir = "/usr/share/pixmaps/";
#endif
#ifdef Q_WS_WIN
    dir = QFileInfo(programEdit->text()).dir().absolutePath();
#endif
    QString fileName = QFileDialog::getOpenFileName(this, tr("アイコンを選択"),
            dir, tr("アイコン (*.png *.jpg *.xpm *.bmp *.gif)"));
    if (!fileName.isEmpty()) {
        m_iconFileName = fileName;
        iconButton->setIcon(QIcon(fileName));
    }
}

void CommandActionDialog::on_selectProgramButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("アイコンを選択"));
    if (!fileName.isEmpty())
        programEdit->setText(fileName);
}

void CommandActionDialog::accept()
{
    QStringList error;
    if (textEdit->text().isEmpty())
        error += tr("アクションの名前を入力して下さい。");
    if (programEdit->text().isEmpty())
        error += tr("プログラムを選択して下さい。");
    if (!error.isEmpty()) {
        QMessageBox::warning(this, tr("エラー"), error.join("\n"));
        return;
    }
    if (!iconButton->icon().isNull())
        m_action->setIcon(m_iconFileName);
    if (!textEdit->text().isEmpty())
        m_action->setText(textEdit->text());
    m_action->setShortcut(QKeySequence(shortcutEdit->text()));
    m_action->setCommand(programEdit->text(), argsEdit->text());

    QDialog::accept();
}

