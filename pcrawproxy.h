/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef PCRAWPROXY_H
#define PCRAWPROXY_H

#include <QtCore>

class PCRawProxy : public QObject
{
    Q_OBJECT
public:
    PCRawProxy(const QString &program, int port, const QUrl &peerCastServerUrl, QObject *parent = 0);
    virtual ~PCRawProxy();

    bool start();
    void stop();
    void restart();

    QString errorString() const;

private slots:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess m_process;
    QString m_program;
    int m_port;
    QUrl m_peerCastServerUrl;
    QString m_errorString;
};

#endif // PCRAWPROXY_H
