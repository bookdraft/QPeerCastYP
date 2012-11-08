/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "mainwindow.h"
#include "application.h"
#include "actions.h"
#include "systemtrayicon.h"
#include "yellowpage.h"
#include "yellowpagemanager.h"
#include "channel.h"
#include "channellistwidget.h"
#include "channellisttabwidget.h"
#include "channellistfindbar.h"
#include "pcrawproxy.h"
#include "network.h"
#include "sound.h"
#include "settings.h"
#include "settingsdialog.h"
#include "generalwidget.h"
#include "yellowpageedit.h"
#include "useractionedit.h"

#include "ui_aboutqpeercastyp.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags), m_autoUpdateEnabled(false)
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::setup()
{
    setWindowTitle(qApp->applicationName());
    setAttribute(Qt::WA_AlwaysShowToolTips);

    setupActions();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();

    connect(&m_autoUpdateTimer, SIGNAL(timeout()), this, SLOT(updateYellowPage()));

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_stackedWidget = new QStackedWidget(centralWidget);
    layout->addWidget(m_stackedWidget);

    m_channelListTabWidget = new ChannelListTabWidget(m_stackedWidget);
    m_channelListTabWidget->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_stackedWidget->addWidget(m_channelListTabWidget);

    m_channelListFindBar = new ChannelListFindBar(centralWidget);
    m_channelListFindBar->hide();
    layout->addWidget(m_channelListFindBar);

    setupChannelListWidget();
    readSettings();
    updateStatusBar();
}

void MainWindow::setupChannelListWidget()
{
    setUpdatesEnabled(false);
    YellowPage *manager = qApp->yellowPageManager();
    m_mergedChannelList = new ChannelListWidget(m_channelListTabWidget, manager);
    m_mergedChannelList->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_channelListTabWidget->addTab(m_mergedChannelList, manager->name());
    m_mergedChannelList->setActive(true);
    connect(m_channelListFindBar, SIGNAL(findRequest(QString, Qt::MatchFlags)),
            m_channelListTabWidget, SLOT(findRequest(QString, Qt::MatchFlags)));

    foreach (YellowPage *yp, manager->yellowPages()) {
        if (!yp->isEnabled())
            continue;
        ChannelListWidget *list = new ChannelListWidget(m_channelListTabWidget, yp);
        list->setAttribute(Qt::WA_MacShowFocusRect, false);
        m_channelListTabWidget->addTab(list, yp->name());
    }
    setUpdatesEnabled(true);
    m_actions->showTabBarAction()->setChecked(false);
}

void MainWindow::clearChannelListWidget()
{
    setUpdatesEnabled(false);
    if (!m_channelListTabWidget->isVisible())
        delete m_mergedChannelList;
    while (m_channelListTabWidget->currentWidget()) {
        ChannelListWidget *widget = m_channelListTabWidget->currentWidget();
        m_channelListTabWidget->removeTab(m_channelListTabWidget->currentIndex());
        delete widget;
    }
    setUpdatesEnabled(true);
}

QList<ChannelListWidget *> MainWindow::channelListWidgets() const
{
    QList<ChannelListWidget *> widgets;
    if (!m_channelListTabWidget->isVisible())
        widgets += m_mergedChannelList;
    for (int i = 0; i < m_channelListTabWidget->count(); ++i)
        widgets += m_channelListTabWidget->widget(i);
    return widgets;
}

void MainWindow::setupActions()
{
    m_actions = new Actions(this);
}

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
void MainWindow::activateWindow()
{
    static Atom NET_ACTIVE_WINDOW = XInternAtom(
            QX11Info::display(), "_NET_ACTIVE_WINDOW", False);
    XClientMessageEvent xev;
    xev.type = ClientMessage;
    xev.window = winId();
    xev.message_type = NET_ACTIVE_WINDOW;
    xev.format = 32;
    xev.data.l[0] = 2;
    xev.data.l[1] = CurrentTime;
    xev.data.l[2] = 0;
    xev.data.l[3] = 0;
    xev.data.l[4] = 0;

    XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False,
        (SubstructureNotifyMask | SubstructureRedirectMask), (XEvent *)&xev);
}
#endif // Q_WS_X11

void MainWindow::setVisible(bool visible)
{
    if (isVisible() == visible)
        return;
    if (visible and !m_pos.isNull())
        move(m_pos);
    if (!visible)
        m_pos = pos();
    QMainWindow::setVisible(visible);
}

void MainWindow::quit()
{
    writeSettings();
    qApp->peercast()->terminate();
    qApp->peercast()->waitForFinished();
    qApp->pcrawProxy()->stop();
    clearChannelListWidget();
    qApp->quit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (qApp->systemTrayIcon()->isVisible()) {
        setVisible(false);
        event->ignore();
    } else {
        quit();
    }
}

void MainWindow::setupMenuBar()
{
    setMenuBar(new QMenuBar(0));
    menuBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    menuBar()->addMenu(m_actions->fileMenu(menuBar()));
    QMenu *menu = m_actions->yellowPageMenu(menuBar());
    menu->setIcon(QIcon());
    menuBar()->addMenu(menu);
    menuBar()->addMenu(m_actions->settingsMenu(menuBar()));
    menuBar()->addMenu(m_actions->helpMenu(menuBar()));
}

void MainWindow::setupToolBar()
{
    m_mainToolBar = addToolBar(tr("メインツールバー"));
    m_mainToolBar->setObjectName("MainToolBar");
    m_mainToolBar->setContextMenuPolicy(Qt::CustomContextMenu);
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    m_mainToolBar->addAction(m_actions->updateYellowPageToolBarAction());
    // m_mainToolBar->addAction(m_actions->playChannelAction());
    m_mainToolBar->addAction(m_actions->openContactUrlAction());
    m_mainToolBar->addAction(m_actions->addToFavoritesAction());
    m_mainToolBar->addAction(m_actions->findChannelAction());
    m_mainToolBar->addAction(m_actions->showSettingsAction());
    connect(m_mainToolBar, SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(toolBarOrientationChanged(Qt::Orientation)));
}

void MainWindow::toolBarOrientationChanged(Qt::Orientation orientation)
{
    QToolBar *toolBar = qobject_cast<QToolBar *>(sender());
    if (orientation == Qt::Horizontal)
        toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    else if (orientation == Qt::Vertical)
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
}

void MainWindow::setupStatusBar()
{
    statusBar()->setSizeGripEnabled(false);
    statusBar()->show();
#ifdef Q_WS_MAC
    QFont f(statusBar()->font());
    f.setPointSize(f.pointSize() - 2);
    statusBar()->setFont(f);
#endif
    connect(statusBar(), SIGNAL(messageChanged(QString)), SLOT(updateStatusBar(QString)));
}

void MainWindow::readSettings()
{
    Settings *settings = qApp->settings();
    resize(settings->value("MainWindow/Size").toSize());
    restoreState(settings->value("MainWindow/State").toByteArray());
    restoreGeometry(settings->value("MainWindow/Geometry").toByteArray());
#ifdef Q_WS_MAC
    setMenuBarVisible(true);
#else
    setMenuBarVisible(settings->value("MainWindow/ShowMenuBar").toBool());
#endif
    setStatusBarVisible(settings->value("MainWindow/ShowStatusBar").toBool());
    setToolBarVisible(settings->value("MainWindow/ShowMainToolBar").toBool());
    setTabBarVisible(settings->value("MainWindow/ShowTabBar").toBool());
    setAutoUpdateEnabled(settings->value("General/AutoUpdate").toBool());
    setAutoUpdateInterval(settings->value("General/AutoUpdateInterval").toInt());
}

void MainWindow::writeSettings()
{
    Settings *settings = qApp->settings();
    settings->setValue("MainWindow/State", saveState());
    settings->setValue("MainWindow/Geometry", saveGeometry());
    settings->setValue("MainWindow/Size", size());
    settings->setValue("MainWindow/ShowMenuBar", m_actions->showMenuBarAction()->isChecked());
    settings->setValue("MainWindow/ShowStatusBar", m_actions->showStatusBarAction()->isChecked());
    settings->setValue("MainWindow/ShowMainToolBar", m_actions->showToolBarAction()->isChecked());
    settings->setValue("MainWindow/ShowTabBar", m_actions->showTabBarAction()->isChecked());
    settings->sync();
}

Actions *MainWindow::actions() const
{
    return m_actions;
}

ChannelListWidget *MainWindow::currentChannelListWidget() const
{
    if (m_channelListTabWidget->isVisible())
        return qobject_cast<ChannelListWidget *>(m_channelListTabWidget->currentWidget());
    else
        return m_mergedChannelList;
}

Channel *MainWindow::currentChannel() const
{
    if (currentChannelListWidget())
        return currentChannelListWidget()->currentChannel();
    else
        return 0;
}

ChannelListTabWidget *MainWindow::channelListTabWidget() const
{
    return m_channelListTabWidget;
}

void MainWindow::updateYellowPage()
{
    if (m_autoUpdateEnabled)
        m_autoUpdateTimer.start();
    if (currentChannelListWidget())
        currentChannelListWidget()->updateYellowPage();
}

void MainWindow::setAutoUpdateEnabled(bool enabled)
{
    m_autoUpdateEnabled = enabled;
    m_actions->toggleAutoUpdateAction()->setChecked(enabled);
    if (m_autoUpdateEnabled) {
        m_autoUpdateTimer.start();
    } else {
        m_autoUpdateTimer.stop();
    }
}

void MainWindow::setAutoUpdateInterval(int sec)
{
    m_autoUpdateTimer.setInterval(sec * 1000);
}

void MainWindow::notifyChannels(YellowPage *yellowPage)
{
    if (!yellowPage) {
        if (currentChannelListWidget())
            yellowPage = currentChannelListWidget()->yellowPage();
        else
            return;
    }
    Settings *settings = qApp->settings();
    ChannelList noticeChannels;
    if (settings->value("Notification/NotifyFavorite").toBool()) {
        noticeChannels += yellowPage->channels(Channel::Favorite | Channel::New);
        if (settings->value("Notification/NotifyChangedFavorite").toBool())
            noticeChannels += yellowPage->channels(Channel::Favorite | Channel::Changed);
        int minimumScore = settings->value("Notification/MinimumScore").toInt();
        foreach (Channel *ch, noticeChannels)
            if (ch->score() < minimumScore)
                noticeChannels.removeAll(ch);
    }
    if (settings->value("Notification/NotifyNew").toBool())
        noticeChannels += yellowPage->channels(Channel::New);
    if (settings->value("Notification/NotifyChanged").toBool())
        noticeChannels += yellowPage->channels(Channel::Changed);
    if (!noticeChannels .isEmpty()) {
        if (settings->value("Notification/ShowBalloonMessage").toBool())
            qApp->systemTrayIcon()->showChannels(noticeChannels);
        if (settings->value("Notification/PlaySound").toBool())
            Sound::play(settings->value("Notification/SoundFile").toString());
    }
}

void MainWindow::playChannel()
{
    if (currentChannelListWidget())
        currentChannelListWidget()->playChannel();
}

void MainWindow::addToFavorites()
{
    if (currentChannelListWidget())
        currentChannelListWidget()->addToFavorites();
}

void MainWindow::openContactUrl()
{
    if (currentChannelListWidget())
        currentChannelListWidget()->openContactUrl();
}

void MainWindow::copyStreamUrl()
{
    if (currentChannelListWidget())
        currentChannelListWidget()->copyStreamUrl();
}

void MainWindow::copyContactUrl()
{
    if (currentChannelListWidget())
        currentChannelListWidget()->copyContactUrl();
}

void MainWindow::copyChannelInfo()
{
    if (currentChannelListWidget())
        currentChannelListWidget()->copyChannelInfo();
}

void MainWindow::findChannel()
{
    m_channelListFindBar->show();
}

void MainWindow::showErrorMessage(const QString &message)
{
    statusBar()->showMessage(message, 5000);
}

void MainWindow::updateStatusBar(const QString &message)
{
    if (message.isNull()) {
        if (ChannelListWidget *list = currentChannelListWidget()) {
            QString m;
            int channelCount = list->channelCount();
            int listenerCount = list->listenerCount();
            int yellowPageCount = list->yellowPageCount();
            if (yellowPageCount > 1) {
                m = tr("チャンネル %1 | 視聴者 %2 | イエローページ %3")
                             .arg(channelCount).arg(listenerCount).arg(yellowPageCount);
            } else {
                m = tr("チャンネル %1 | 視聴者 %2").arg(channelCount).arg(listenerCount);
            }
            statusBar()->showMessage(m);
        }
    } else if (statusBar()->currentMessage() != message) {
        statusBar()->showMessage(message, 5000);
    }
}

void MainWindow::setStatusBarVisible(bool shown)
{
    statusBar()->setVisible(shown);
    m_actions->showStatusBarAction()->setChecked(shown);
}

void MainWindow::setMenuBarVisible(bool shown)
{
    menuBar()->setVisible(shown);
    m_actions->showMenuBarAction()->setChecked(shown);
}

void MainWindow::setToolBarVisible(bool shown)
{
    m_mainToolBar->setVisible(shown);
    m_actions->showToolBarAction()->setChecked(shown);
}

void MainWindow::setTabBarVisible(bool shown)
{
    if (shown == !m_channelListTabWidget->isHidden())
        return;
    setUpdatesEnabled(false);
    if (shown) {
        connect(m_channelListFindBar, SIGNAL(findRequest(QString, Qt::MatchFlags)),
                m_channelListTabWidget, SLOT(findRequest(QString, Qt::MatchFlags)));
        disconnect(m_channelListFindBar, SIGNAL(findRequest(QString, Qt::MatchFlags)),
                m_mergedChannelList, SLOT(findItems(QString, Qt::MatchFlags)));
        m_stackedWidget->setCurrentWidget(m_channelListTabWidget);
        m_stackedWidget->removeWidget(m_mergedChannelList);
        m_channelListTabWidget->insertTab(0, m_mergedChannelList,
                m_mergedChannelList->yellowPage()->name());
        m_channelListTabWidget->setCurrentWidget(m_mergedChannelList);
        m_channelListTabWidget->setActive(true);
    } else {
        connect(m_channelListFindBar, SIGNAL(findRequest(QString, Qt::MatchFlags)),
                m_mergedChannelList, SLOT(findItems(QString, Qt::MatchFlags)));
        disconnect(m_channelListFindBar, SIGNAL(findRequest(QString, Qt::MatchFlags)),
                m_channelListTabWidget, SLOT(findRequest(QString, Qt::MatchFlags)));
        int index = m_channelListTabWidget->indexOf(m_mergedChannelList);
        m_channelListTabWidget->removeTab(index);
        m_stackedWidget->addWidget(m_mergedChannelList);
        m_stackedWidget->setCurrentWidget(m_mergedChannelList);
        m_channelListTabWidget->setActive(false);
        m_mergedChannelList->setActive(true);
    }
    setUpdatesEnabled(true);
    m_actions->showTabBarAction()->setChecked(shown);
    updateStatusBar();
}

void MainWindow::showSettings(SettingsDialog::WidgetIndex index)
{
    setVisible(true);
    Settings *settings = qApp->settings();
    SettingsDialog dialog(settings, this);
    dialog.setCurrentWidget(index);
    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.generalWidget()->isDirty()) {
            setAutoUpdateInterval(settings->value("General/AutoUpdateInterval").toInt());
            qApp->systemTrayIcon()->setVisible(settings->value("SystemTrayIcon/Enabled").toBool());
            foreach (ChannelListWidget *w, channelListWidgets()) {
                w->setCustomToolTip(settings->value("ChannelListWidget/CustomToolTip").toBool());
                w->setLinkEnabled(settings->value("ChannelListWidget/LinkEnabled").toBool()); 
                w->setLinkType(settings->value("ChannelListWidget/LinkType").toInt()); 
            }
        }
        if (dialog.yellowPageEdit()->isDirty()) {
            bool hidden = m_channelListTabWidget->isHidden();
            clearChannelListWidget();
            qApp->yellowPageManager()->clear();
            qApp->yellowPageManager()->loadYellowPages();
            setupChannelListWidget();
            setTabBarVisible(!hidden);
        }
        if (dialog.userActionEdit()->isDirty()) {
            qApp->actions()->loadUserActions();
            setupMenuBar();
        }
    }
}

void MainWindow::aboutQt()
{
    QApplication::aboutQt();
}

void MainWindow::aboutQPeerCastYP()
{
    QDialog dialog(this);
    Ui::AboutQPeerCastYP ui;
    ui.setupUi(&dialog);
    ui.iconLabel->setPixmap(qApp->applicationIcon().pixmap(64, 64));
    ui.label->setText("<center><p><b>QPeerCastYP</b></p></center>"
                      "<center><p>バージョン " VERSION "</p></center>"
                      "<center>© 2008-2012 bookdraft<center>");
    dialog.resize(dialog.minimumSizeHint().width() + 200, dialog.minimumSizeHint().height());
    dialog.exec();
}

