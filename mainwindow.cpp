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
    setupActions();
    setupMenus();
    setupToolBar();
    setupStatusBar();

    connect(&m_autoUpdateTimer, SIGNAL(timeout()), this, SLOT(updateYellowPage()));

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *layout = new QHBoxLayout(centralWidget);
    layout->setMargin(0);
    layout->setSpacing(0);

    QVBoxLayout *tabBarLayout = new QVBoxLayout;
    layout->addLayout(tabBarLayout);
    QTabBar *tabBar = new QTabBar(centralWidget);
    tabBarLayout->addWidget(tabBar);
    tabBarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    tabBar->setShape(QTabBar::RoundedWest);

    // イエローページ
    tabBar->addTab(tr("チャンネル一覧"));
    QStackedWidget *stackedWidget = new QStackedWidget(centralWidget);
    layout->addWidget(stackedWidget);
    connect(tabBar, SIGNAL(currentChanged(int)),
            stackedWidget, SLOT(setCurrentIndex(int)));

    QWidget *yellowPageWidget = new QWidget(centralWidget);
    QVBoxLayout *yellowPageLayout = new QVBoxLayout;
    yellowPageLayout->setMargin(0);
    yellowPageLayout->setSpacing(0);
    stackedWidget->addWidget(yellowPageWidget);
    yellowPageWidget->setLayout(yellowPageLayout);
    m_channelListTabWidget = new ChannelListTabWidget(centralWidget);
    yellowPageLayout->addWidget(m_channelListTabWidget);
    m_channelListFindBar = new ChannelListFindBar(centralWidget);
    m_channelListFindBar->hide();
    yellowPageLayout->addWidget(m_channelListFindBar);

    setupChannelListWidget();

    // PeerCast
    tabBar->addTab(tr("PeerCast"));
    QWidget *peercast = new QWidget(centralWidget);
    layout->addWidget(peercast);
    stackedWidget->addWidget(peercast);

    tabBar->hide();

    readSettings();
}

void MainWindow::setupChannelListWidget()
{
    YellowPage *manager = qApp->yellowPageManager();
    ChannelListWidget *list = new ChannelListWidget(m_channelListTabWidget, manager);
    m_channelListTabWidget->addTab(list, manager->name());
    list->setActive(true);
    connect(m_channelListFindBar, SIGNAL(findRequest(QString, Qt::MatchFlags)),
            m_channelListTabWidget, SLOT(findRequest(QString, Qt::MatchFlags)));

    foreach (YellowPage *yp, manager->yellowPages()) {
        if (!yp->isEnabled())
            continue;
        list = new ChannelListWidget(m_channelListTabWidget, yp);
        m_channelListTabWidget->addTab(list, yp->name());
    }
}

void MainWindow::clearChannelListWidget()
{
    while (m_channelListTabWidget->currentWidget()) {
        ChannelListWidget *widget = m_channelListTabWidget->currentWidget();
        m_channelListTabWidget->removeTab(m_channelListTabWidget->currentIndex());
        delete widget;
    }
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
    hide();
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

void MainWindow::setupMenus()
{
    menuBar()->show();
    menuBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    menuBar()->addMenu(m_actions->fileMenu(this));
    menuBar()->addMenu(m_actions->yellowPageMenu(this));
    menuBar()->addMenu(m_actions->settingsMenu(this));
    menuBar()->addMenu(m_actions->helpMenu(this));
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
    statusBar()->show();
    connect(statusBar(), SIGNAL(messageChanged(QString)), SLOT(updateStatusBar(QString)));
}

void MainWindow::readSettings()
{
    Settings *settings = qApp->settings();
    resize(settings->value("MainWindow/Size").toSize());
    restoreState(settings->value("MainWindow/State").toByteArray());
    setMenuBarVisible(settings->value("MainWindow/ShowMenuBar").toBool());
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
    settings->setValue("MainWindow/Size", size());
    settings->setValue("MainWindow/ShowMenuBar", m_actions->showMenuBarAction()->isChecked());
    settings->setValue("MainWindow/ShowStatusBar", m_actions->showStatusBarAction()->isChecked());
    settings->setValue("MainWindow/ShowMainToolBar", m_actions->showToolBarAction()->isChecked());
    settings->setValue("MainWindow/ShowTabBar", m_actions->showTabBarAction()->isChecked());
}

Actions *MainWindow::actions() const
{
    return m_actions;
}

ChannelListWidget *MainWindow::currentChannelListWidget() const
{
    return qobject_cast<ChannelListWidget *>(m_channelListTabWidget->currentWidget());
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
            QString message;
            int channelCount = list->channelCount();
            int listenerCount = list->listenerCount();
            int yellowPageCount = list->yellowPageCount();
            if (yellowPageCount > 1) {
                message = tr("%1 チャンネル（合計 %2 リスナー、%3 イエローページ）")
                             .arg(channelCount).arg(listenerCount).arg(yellowPageCount);
            } else {
                message = tr("%1 チャンネル（合計 %2 リスナー）").arg(channelCount).arg(listenerCount);
            }
            statusBar()->showMessage(message);
        }
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
    m_channelListTabWidget->setTabBarVisible(shown);
    m_actions->showTabBarAction()->setChecked(shown);
}

void MainWindow::showSettings()
{
    setVisible(true);
    Settings *settings = qApp->settings();
    SettingsDialog dialog(settings, this);
    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.generalWidget()->isDirty()) {
            setAutoUpdateInterval(settings->value("General/AutoUpdateInterval").toInt());
            qApp->systemTrayIcon()->setVisible(settings->value("SystemTrayIcon/Enabled").toBool());
        }
        if (dialog.yellowPageEdit()->isDirty()) {
            m_channelListTabWidget->setUpdatesEnabled(false);
            clearChannelListWidget();
            qApp->yellowPageManager()->clear();
            qApp->yellowPageManager()->loadYellowPages();
            setupChannelListWidget();
            m_channelListTabWidget->setUpdatesEnabled(true);
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
    ui.iconLabel->setPixmap(qApp->windowIcon().pixmap(64, 64));
    ui.label->setText("<p><b>QPeerCastYP " VERSION "</b></p>"
                      "QPeerCastYP は、PeerCast の視聴を補助するプログラムです。"
                      "Linux と Windows 上で動作します。");
    dialog.resize(dialog.minimumSizeHint().width() + 200, dialog.minimumSizeHint().height());
    dialog.exec();
}

