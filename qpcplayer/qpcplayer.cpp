/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "qpcplayer.h"

QPCPlayer::QPCPlayer(const QStringList &args, QWidget *parent)
    : QWidget(parent), m_args(args), m_retryCount(0),
      m_framelessWindow(false), m_videoAspect(0)
{
    m_videoSize = QSize(320, 240);
    setVideoAspect();
    m_process = new QProcess(this);
    m_process->setTextModeEnabled(true);
    m_process->setReadChannelMode(QProcess::MergedChannels);

    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(backgroundRole(), QColor("black"));
    setPalette(p);

    setAttribute(Qt::WA_NoSystemBackground);
    // setAttribute(Qt::WA_StaticContents);
    // setAttribute(Qt::WA_OpaquePaintEvent);
    // setAttribute(Qt::WA_PaintOnScreen);
    // setAttribute(Qt::WA_PaintUnclipped);
    installEventFilter(this);

    // setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding);
    // setFocusPolicy(Qt::StrongFocus);
    connect(m_process, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(finished()));

    new QShortcut(QKeySequence("Ctrl+Q"), this, SLOT(close()));
    new QShortcut(QKeySequence("Q"), this, SLOT(close()));
    QAction *action = new QAction(this);
    action->setShortcut(QKeySequence("F"));
    connect(action, SIGNAL(triggered(bool)), SLOT(showFullScreenOrNormal()));
    addAction(action);

    resize(m_videoSize);

    connect(this, SIGNAL(videoResolutionRecieved(const QSize &)), SLOT(initSize(const QSize &)));

    start();
}

QPCPlayer::~QPCPlayer()
{
}

void QPCPlayer::start()
{
    QString mplayer("mplayer");
    QStringList arguments = m_args;
    arguments << "-dr" << "-v" << "-slave" << "-quiet" << "-zoom" << "-vo" << "x11"
              << "-wid" << QString::number(winId());
    QPoint oldPos = pos();
    m_process->start(mplayer, arguments, QIODevice::ReadWrite | QIODevice::Unbuffered);
    m_process->waitForStarted();
    move(oldPos);
}

void QPCPlayer::stop()
{
    command("quit");
    m_process->terminate();
    m_process->waitForFinished();
    if (m_process->state() == QProcess::Running)
        m_process->kill();
}

void QPCPlayer::command(const QString &command)
{
    m_process->write(command.toLatin1() + "\n");
}

void QPCPlayer::mouseDoubleClickEvent(QMouseEvent *event)
{
    showFullScreenOrNormal();
    QWidget::mouseDoubleClickEvent(event);
}

void QPCPlayer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_1)  {
        resize(m_videoSize);
        return;
    }
    QWidget::keyPressEvent(event);
}

void QPCPlayer::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton and !isFullScreen())
        move(event->globalPos() - m_pressPos);
    QWidget::mouseMoveEvent(event);
}

void QPCPlayer::closeEvent(QCloseEvent *event)
{
    hide();
    stop();
    QWidget::closeEvent(event);
}

bool QPCPlayer::eventFilter(QObject *obj, QEvent *event)
{
    QMouseEvent *me = static_cast<QMouseEvent *>(event);
    if (event->type() == QEvent::MouseButtonPress) {
        if (me->button() & Qt::LeftButton) {
            m_pressPos = me->globalPos() - pos();
            if (!isFullScreen())
                QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        } else if (me->button() & Qt::MidButton) {
            resize(m_videoSize);
        }
        return true;
    }
    if (event->type() == QEvent::MouseButtonRelease
            and me->button() & Qt::LeftButton) {
        if (!isFullScreen())
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        return true;
    }
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *we = static_cast<QWheelEvent *>(event);
        int delta = we->delta();
        if (we->buttons() & Qt::LeftButton) {
            if (delta < 0)
                command("volume -1");
            else
                command("volume +1");
        } else {
            if (m_videoAspect != 0) {
                double w = delta < 0 ? size().width() / 1.04 : size().width() * 1.04;
                resize((int)w, (int)(w / m_videoAspect));
            }
        }
        return true;
    }

    return QWidget::eventFilter(obj, event);
}

void QPCPlayer::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.eraseRect(e->rect());
    // painter.fillRect(e->rect(), QBrush("black"));
}

void QPCPlayer::showFullScreenOrNormal()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        command("fs");
        showFullScreen();
        qDebug() << QApplication::desktop()->size();
    }
    repaint();
}

void QPCPlayer::finished()
{
    if (m_retryCount < 32) {
        m_retryCount++;
        sleep(3);
        start();
    }
}

void QPCPlayer::initSize(const QSize &size)
{
    resize(size);
}

void QPCPlayer::setVideoAspect()
{
    m_videoAspect = (double)m_videoSize.width() / (double)m_videoSize.height();
}

void QPCPlayer::readyRead()
{
    // QRegExp rxVideoRes("ANS_VIDEO_RESOLUTION='(\\d+) x (\\d+)'");
    QRegExp rxVideoWidth("ANS_width=(\\d+)");
    QRegExp rxVideoHeight("ANS_height=(\\d+)");
    QRegExp rxVideoAspect("ANS_aspect=([\\d.]+)");
    while (m_process->canReadLine()) {
        QString line = m_process->readLine();
        line.chop(1);
        qDebug() << line;
        if (line.startsWith("Starting playback...")) {
            // command("get_video_resolution");
            command("get_property width");
            command("get_property height");
            command("get_property aspect");
        }
        if (rxVideoWidth.indexIn(line) != -1) {
            m_videoSize.setWidth(rxVideoWidth.cap(1).toInt());
        } else if (rxVideoHeight.indexIn(line) != -1) {
            m_videoSize.setHeight(rxVideoHeight.cap(1).toInt());
        } else if (rxVideoAspect.indexIn(line) != -1) {
            m_videoAspect = rxVideoAspect.cap(1).toDouble();
            if (m_videoAspect)
                m_videoSize.setHeight((int)(m_videoSize.width() / m_videoAspect));
            else
                setVideoAspect();
            qDebug() << m_videoSize;
            emit videoResolutionRecieved(m_videoSize);
        }
            // disconnect(m_process, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }
}
