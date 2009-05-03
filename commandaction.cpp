/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "commandaction.h"
#include "mainwindow.h"
#include "application.h"
#include "actions.h"
#include "process.h"
#include "utils.h"

CommandAction::CommandAction(const QString &text, QObject *parent)
    : QAction(text, parent)
{
    connect(this, SIGNAL(triggered(bool)), this, SLOT(actionTriggered()));
}

CommandAction::~CommandAction()
{
}

void CommandAction::setEnabled(bool enabled)
{
    QAction::setEnabled(enabled);
    QAction::setVisible(enabled);
}

QString CommandAction::program() const
{
    return m_program;
}

QString CommandAction::arguments() const
{
    return m_args;
}

void CommandAction::setCommand(const QString &program, const QString &args)
{
    m_program = program;
    m_args = args;
}

void CommandAction::setIcon(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    QAction::setIcon(QIcon(fileName));
    m_iconFileName = fileName;
}

QString CommandAction::iconFileName() const
{
    return m_iconFileName;
}

void CommandAction::actionTriggered()
{
    if (m_program.isEmpty())
        return;
    Channel *ch = qApp->mainWindow()->currentChannel();
    qApp->actions()->startProcess(m_program, Utils::shellwords(m_args), ch);
}

