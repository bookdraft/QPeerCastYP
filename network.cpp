/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "network.h"

QString Network::s_userAgent = "Mozilla/4.0";

Network::Network(QObject *parent)
    : QObject(parent)
{
}

QString Network::userAgent()
{
    return s_userAgent;
}

void Network::setUserAgent(const QString &userAgent)
{
    s_userAgent = userAgent;
}

int Network::get(QHttp *http, const QUrl &url)
{
    http->setHost(url.host(), url.port(80));
    QString path = url.path();
    if (url.hasQuery())
        path += "?" + url.encodedQuery();
    QHttpRequestHeader header("GET", path);
    header.setValue("User-Agent", s_userAgent);
    header.setValue("Host", url.host());
    return http->request(header);
}

