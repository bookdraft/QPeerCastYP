/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef NETWORK_H
#define NETWORK_H

#include <QtCore>
#include <QtNetwork>

class Network : public QObject
{
    Q_OBJECT
public:
    Network(QObject *parent = 0);

    static int get(QHttp *http, const QUrl &url);
    static QString userAgent();
    static void setUserAgent(const QString &userAgent);

private:
    static QString s_userAgent;
};

#endif // NETWORK_H
