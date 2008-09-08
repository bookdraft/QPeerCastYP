/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "pcrawproxy.h"
#include "application.h"
#include "mainwindow.h"

PCRawProxy::PCRawProxy(const QString &program, int port, const QUrl &peerCastServerUrl, QObject *parent)
    : QObject(parent), m_program(program), m_port(port), m_peerCastServerUrl(peerCastServerUrl)
{
}

PCRawProxy::~PCRawProxy()
{
}

QString PCRawProxy::errorString() const
{
    return m_errorString;
}

bool PCRawProxy::start()
{
    connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(finished(int, QProcess::ExitStatus)));
    QStringList args;
    args += QString::number(m_port);
    args += QString("%1:%2").arg(m_peerCastServerUrl.host()).arg(m_peerCastServerUrl.port());
    m_process.start(m_program, args);
    if (!m_process.waitForStarted()) {
        QMessageBox::warning(qApp->mainWindow(), tr("エラー"),
                tr("PCRaw プロキシサーバの起動に失敗しました。\n"
                   "PCRaw プロキシサーバプログラムの場所を確認して下さい。"));
        return false;
    }
    return true;
}

void PCRawProxy::stop()
{
    disconnect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
               this, SLOT(finished(int, QProcess::ExitStatus)));
    m_process.terminate();
    if (!m_process.waitForFinished(400))
        m_process.kill();
}

void PCRawProxy::restart()
{
    stop();
    start();
}

void PCRawProxy::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode) {
        QMessageBox::warning(qApp->mainWindow(), tr("エラー"),
                tr("PCRaw プロキシサーバの起動に失敗しました。\n"
                   "既に PCRaw プロキシサーバが起動しているかもしれません。"));
    } else if (exitStatus == QProcess::CrashExit) {
        int button = QMessageBox::question(qApp->mainWindow(), tr("PCRaw プロキシサーバを再起動しますか？"),
                        tr("PCRaw プロキシサーバがクラッシュしました。\nPCRaw プロキシサーバを起動しなおしますか？"),
                        QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::Yes)
            start();
    }
}

