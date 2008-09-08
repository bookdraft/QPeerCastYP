/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "systemtrayicon.h"
#include "application.h"
#include "mainwindow.h"
#include "settings.h"
#include "actions.h"
#include "channel.h"

SystemTrayIcon::SystemTrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
{
    setIcon(qApp->windowIcon());
    QMenu *menu = new QMenu;
    Actions *actions = qApp->actions();
    menu->addAction(actions->updateYellowPageAction());
    menu->addAction(actions->toggleAutoUpdateAction());
    menu->addAction(actions->showSettingsAction());
    menu->addSeparator();
    menu->addAction(actions->quitAction());
    setContextMenu(menu);
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(activated(QSystemTrayIcon::ActivationReason)));
    connect(this, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
}

SystemTrayIcon::~SystemTrayIcon()
{
}

bool scoreMoreThan(Channel *c1, Channel *c2)
{
    return c1->score() > c2->score();
}

void SystemTrayIcon::showFavoriteChannels(const QList<Channel *> &channels_)
{
    if (!isVisible())
        return;
    ChannelList channels = channels_;
    if (!channels.isEmpty()) {
        QStringList message;
        qSort(channels.begin(), channels.end(), scoreMoreThan);
        foreach (Channel *channel, channels) {
            if (channel->hasScore()) {
                message += QString("%1 (スコア: %2)").arg(channel->name()).arg(channel->score());
                message += channel->longDescription();
                if (!message.last().isEmpty())
                    message.last().append("\n");
            }
        }
        showMessage(tr("お気に入り通知"), message.join("\n").remove(QRegExp("\n$")));
    }
}

void SystemTrayIcon::activated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        qApp->mainWindow()->setVisible(!qApp->mainWindow()->isVisible());
    } else if (reason == QSystemTrayIcon::Context) {
        contextMenu()->exec(QCursor::pos());
    }
}

void SystemTrayIcon::messageClicked()
{
    qApp->mainWindow()->setVisible(true);
    // qApp->mainWindow()->activateWindow();
}

