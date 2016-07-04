/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef COMMANDACTIONDIALOG_H
#define COMMANDACTIONDIALOG_H

#include <QtGui>
#include "ui_commandactiondialog.h"

class CommandAction;

class CommandActionDialog : public QDialog, private Ui::CommandActionDialog
{
    Q_OBJECT
public:
    CommandActionDialog(CommandAction *action, QWidget *parent = 0);
    virtual ~CommandActionDialog();

public slots:
    void accept();

private slots:
    void on_iconButton_clicked();
    void on_selectProgramButton_clicked();

private:

    CommandAction *m_action;
    QString m_iconFileName;
};

#endif // COMMANDACTIONDIALOG_H
