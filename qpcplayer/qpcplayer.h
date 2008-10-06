/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef QPCPLAYER_H
#define QPCPLAYER_H

#include <QtGui>
#include <QX11EmbedContainer>

class QPCPlayer : public QWidget
{
    Q_OBJECT
public:
    QPCPlayer(const QStringList &args, QWidget *parent = 0);
    virtual ~QPCPlayer();

    void start();
    void stop();
    void command(const QString &command);

signals:
    void videoResolutionRecieved(const QSize &size);

public slots:
    void initSize(const QSize &size);
    void showFullScreenOrNormal();
    void finished();
    void readyRead();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *, QEvent *event);
    void paintEvent(QPaintEvent *e);

private:
    void setVideoAspect();

    QStringList m_args;
    QSize m_videoSize;
    int m_retryCount;
    QPoint m_pressPos;
    bool m_framelessWindow;
    double m_videoAspect;
    QProcess *m_process;
};

#endif // QPCPLAYER_H
