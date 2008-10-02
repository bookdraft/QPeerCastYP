/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef COMMANDACTION_H
#define COMMANDACTION_H

#include <QtGui>

class CommandAction : public QAction
{
    Q_OBJECT
public:
    CommandAction(const QIcon &icon, const QString &text, QObject *parent);
    virtual ~CommandAction();

    void setCommand(const QString &program, const QString &args);

private slots:
    void actionTriggered();

private:
    QString m_program;
    QString m_args;
};

#endif // COMMANDACTION_H
