/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef UTILS_H
#define UTILS_H

#include <QtCore>

namespace Utils
{
    QString unescape(const QString &str);
    QString wsString();
    QString osString();

    QStringList shellwords(const QString &line);
}

#endif // UTILS_H
