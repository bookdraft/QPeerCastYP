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
    setIcon(qApp->applicationIcon());
    QMenu *menu = new QMenu(qApp->mainWindow());
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

bool scoreGreaterThan(Channel *c1, Channel *c2)
{
    return c1->score() > c2->score();
}

void SystemTrayIcon::showChannels(const QList<Channel *> &channels_)
{
    ChannelList channels = channels_;
    if (!channels.isEmpty()) {
        QStringList message;
        qSort(channels.begin(), channels.end(), scoreGreaterThan);
        for (int i = 0; i < channels.count(); i++) {
            int index = i + 1;
            if (index > channels.count())
                break;
            while ((index = channels.indexOf(channels[i], index)) != -1)
                channels.removeAt(index);
        }
        int max = qApp->settings()->value("Notification/MaximumShowChannels").toInt();
        for (int i = 0; i < channels.count() and i < max; i++) {
            Channel *channel = channels[i];
            QStringList status;
            if (channel->status() & Channel::New)
                status += "新規";
            else if (channel->status() & Channel::Changed)
                status += "詳細変更";
            if (channel->hasScore())
                status += QString("スコア %1").arg(channel->score());
            message += QString("%1 (%2)").arg(channel->name()).arg(status.join(" / "));
            message += channel->longDescription().prepend(" ");
            if (!message.last().simplified().isEmpty())
                message.last().append("\n");
        }
        if (!message.isEmpty())
            showMessage(tr("チャンネル通知"), message.join("\n").remove(QRegExp("\n$")));
    }
}

void SystemTrayIcon::activated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
#ifdef Q_WS_X11
        if (qApp->mainWindow()->isActiveWindow()) {
            qApp->mainWindow()->setVisible(false);
        } else {
            qApp->mainWindow()->setVisible(true);
            qApp->mainWindow()->raise();
            qApp->mainWindow()->activateWindow();
        }
#else
        bool visible = !qApp->mainWindow()->isVisible();
        qApp->mainWindow()->setVisible(visible);
        if (visible) {
            qApp->mainWindow()->raise();
            qApp->mainWindow()->activateWindow();
        }
#endif // Q_WS_X11
    } else if (reason == QSystemTrayIcon::Context) {
        contextMenu()->exec(QCursor::pos());
    }
}

void SystemTrayIcon::messageClicked()
{
    qApp->mainWindow()->setVisible(true);
    qApp->mainWindow()->raise();
    qApp->mainWindow()->activateWindow();
}

