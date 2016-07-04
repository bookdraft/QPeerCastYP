/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QtGui>

class Channel;

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    SystemTrayIcon(QObject *parent = 0);
    virtual ~SystemTrayIcon();

    void showChannels(const QList<Channel *> &channels);
    void showMainWindow() const;

public slots:
    void activated(QSystemTrayIcon::ActivationReason reason);

private slots:
    void messageClicked();
};

#endif // SYSTEMTRAYICON_H
