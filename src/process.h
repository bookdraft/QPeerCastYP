/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef PROCESS_H
#define PROCESS_H

#include <QtCore>

class Channel;

class Process : public QProcess
{
    Q_OBJECT
public:
    static bool start(const QString &program, Channel *channel = 0);
    static bool start(const QString &program, const QStringList &arguments, Channel *channel = 0);
    static QString expandVars(const QString &str, Channel *channel = 0);
};

#endif // PROCESS_H
