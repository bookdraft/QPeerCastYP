/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "actions.h"
#include "application.h"
#include "mainwindow.h"
#include "channellistwidget.h"
#include "channel.h"
#include "useractions.h"
#include "commandaction.h"
#include "process.h"
#include "settings.h"
#include "utils.h"

Actions::Actions(MainWindow *mainWindow)
    : QObject(mainWindow), m_mainWindow(mainWindow), m_userActions(0)
{
    // ファイル
    m_quitAction = new QAction(QIcon(":/images/exit.png"), tr("終了(&Q)"), this);
    m_quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
    m_quitAction->setMenuRole(QAction::QuitRole);
    m_mainWindow->addAction(m_quitAction);
    connect(m_quitAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(quit()));

    // イエローページ
    m_updateYellowPageAction = new QAction(QIcon(":/images/reload.png"), tr("イエローページを更新(&R)"), this);
    m_updateYellowPageAction->setShortcut(QKeySequence("Ctrl+R"));
    m_mainWindow->addAction(m_updateYellowPageAction);
    connect(m_updateYellowPageAction, SIGNAL(changed()), this, SLOT(updateYellowPageActionChanged()));
    connect(m_updateYellowPageAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(updateYellowPage()));

    m_toggleAutoUpdateAction = new QAction(QIcon(), tr("自動更新を有効に(&A)"), this);
    m_toggleAutoUpdateAction->setCheckable(true);
    m_mainWindow->addAction(m_toggleAutoUpdateAction);
    connect(m_toggleAutoUpdateAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setAutoUpdateEnabled(bool)));

    m_updateYellowPageToolBarAction = new QAction(m_updateYellowPageAction->icon(),
                                                  m_updateYellowPageAction->text(), this);
    m_updateYellowPageToolBarAction->setIconText(tr("更新"));
#ifndef Q_WS_MAC
    m_updateYellowPageToolBarAction->setMenu(new QMenu(m_mainWindow));
    m_updateYellowPageToolBarAction->menu()->addAction(m_toggleAutoUpdateAction);
#endif
    m_mainWindow->addAction(m_updateYellowPageAction);
    connect(m_updateYellowPageToolBarAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(updateYellowPage()));

    m_playChannelAction = new QAction(QIcon(":/images/play.png"), tr("チャンネルを再生(&P)"), this);
    m_playChannelAction->setIconText(tr("再生"));
    m_mainWindow->addAction(m_playChannelAction);
    connect(m_playChannelAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(playChannel()));

    m_addToFavoritesAction = new QAction(QIcon(":/images/favorite.png"), tr("お気に入りに追加(&A)"), this);
    m_addToFavoritesAction->setIconText(tr("お気に入り"));
    m_addToFavoritesAction->setShortcut(QKeySequence("Ctrl+D"));
    m_mainWindow->addAction(m_addToFavoritesAction);
    connect(m_addToFavoritesAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(addToFavorites()));

    m_openContactUrlAction = new QAction(QIcon(":/images/browser.png"), tr("コンタクトURLをウェブ・ブラウザで開く(&W)"), this);
    m_openContactUrlAction->setIconText(tr("コンタクト"));
    m_openContactUrlAction->setShortcut(QKeySequence("Ctrl+Return"));
    m_mainWindow->addAction(m_openContactUrlAction);
    connect(m_openContactUrlAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(openContactUrl()));

    m_openContactUrlWith2chBrowserAction = new QAction(tr("コンタクトURLを&2ちゃんねるブラウザで開く"), this);
    m_openContactUrlWith2chBrowserAction->setShortcut(QKeySequence("Shift+Return"));
    m_mainWindow->addAction(m_openContactUrlWith2chBrowserAction);
    // connect(m_openContactUrlWith2chBrowserAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(openContactUrlWith2chBrowser()));

    m_copyChannelInfoAction = new QAction(QIcon(":/images/copy.png"), tr("チャンネル情報をコピー(&I)"), this);
    m_copyChannelInfoAction->setShortcut(QKeySequence("Ctrl+C,Ctrl+C"));
    m_mainWindow->addAction(m_copyChannelInfoAction);
    connect(m_copyChannelInfoAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(copyChannelInfo()));

    m_copyStreamUrlAction = new QAction(QIcon(":/images/copy.png"), tr("ストリームURLをコピー(&S)"), this);
    m_copyStreamUrlAction->setShortcut(QKeySequence("Ctrl+C,Ctrl+S"));
    m_mainWindow->addAction(m_copyStreamUrlAction);
    connect(m_copyStreamUrlAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(copyStreamUrl()));

    m_copyContactUrlAction = new QAction(QIcon(":/images/copy.png"), tr("コンタクトURLをコピー(&C)"), this);
    m_copyContactUrlAction->setShortcut(QKeySequence("Ctrl+C,Ctrl+T"));
    m_mainWindow->addAction(m_copyContactUrlAction);
    connect(m_copyContactUrlAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(copyContactUrl()));

    m_findChannelAction = new QAction(QIcon(":/images/find.png"), tr("チャンネルを検索(&F)"), this);
    m_findChannelAction->setIconText(tr("検索"));
    m_findChannelAction->setShortcut(QKeySequence("Ctrl+F"));
    m_mainWindow->addAction(m_findChannelAction);
    connect(m_findChannelAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(findChannel()));

    // 設定
    m_showMenuBarAction = new QAction(tr("メニューバーを表示(&M)"), this);
    m_showMenuBarAction->setCheckable(true);
    m_showMenuBarAction->setShortcut(QKeySequence("Ctrl+M"));
    m_mainWindow->addAction(m_showMenuBarAction);
    connect(m_showMenuBarAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setMenuBarVisible(bool)));

    m_showToolBarAction = new QAction(tr("ツールバーを表示(&T)"), this);
    m_showToolBarAction->setCheckable(true);
    m_mainWindow->addAction(m_showToolBarAction);
    connect(m_showToolBarAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setToolBarVisible(bool)));

    m_showStatusBarAction = new QAction(tr("ステータスバーを表示(&A)"), this);
    m_showStatusBarAction->setCheckable(true);
    m_mainWindow->addAction(m_showStatusBarAction);
    connect(m_showStatusBarAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setStatusBarVisible(bool)));

    m_showTabBarAction = new QAction(tr("タブバーを表示(&B)"), this);
    m_showTabBarAction->setShortcut(QKeySequence("Ctrl+T"));
    m_showTabBarAction->setCheckable(true);
    m_mainWindow->addAction(m_showTabBarAction);
    connect(m_showTabBarAction, SIGNAL(toggled(bool)), m_mainWindow, SLOT(setTabBarVisible(bool)));

    m_showSettingsAction = new QAction(QIcon(":/images/configure.png"), tr("%1 を設定(&C)...").arg(QApplication::applicationName()), this);
    m_showSettingsAction->setIconText(tr("設定"));
    m_showSettingsAction->setMenuRole(QAction::PreferencesRole);
    m_mainWindow->addAction(m_showSettingsAction);
    connect(m_showSettingsAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(showSettings()));

    // ヘルプ
    m_aboutQPeerCastYPAction = new QAction(tr("Q&PeerCastYP について"), this);
    m_aboutQPeerCastYPAction->setIcon(QApplication::windowIcon());
    m_aboutQPeerCastYPAction->setMenuRole(QAction::AboutRole);
    m_mainWindow->addAction(m_aboutQPeerCastYPAction);
    connect(m_aboutQPeerCastYPAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(aboutQPeerCastYP()));

    m_aboutQtAction = new QAction(tr("&Qt について"), this);
    m_mainWindow->addAction(m_aboutQtAction);
    connect(m_aboutQtAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(aboutQt()));

    loadUserActions();
}

Actions::~Actions()
{
}

void Actions::loadUserActions()
{
    if (m_userActions) {
        foreach (QAction *action, m_userActions->actions())
            m_mainWindow->removeAction(action);
        m_userActions->loadActions();
    } else {
        m_userActions = new UserActions(qApp->settings(), this);
    }
    foreach (QAction *action, m_userActions->actions())
        m_mainWindow->addAction(action);
}

QMenu *Actions::fileMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu(tr("ファイル(&F)"), parent);
    menu->addAction(m_quitAction);
    return menu;
}

QMenu *Actions::yellowPageMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu(tr("イエローページ(&Y)"), parent);
    menu->addAction(m_updateYellowPageAction);
    menu->addAction(m_toggleAutoUpdateAction);
    menu->addAction(m_playChannelAction);
    if (!m_userActions->actions().isEmpty()) {
        menu->addSeparator();
        foreach (QAction *action, m_userActions->actions())
            menu->addAction(action);
    }
    menu->addSeparator();
    menu->addAction(m_addToFavoritesAction);
    menu->addSeparator();
    menu->addAction(m_openContactUrlAction);
    menu->addSeparator();
    menu->addAction(m_copyChannelInfoAction);
    menu->addAction(m_copyStreamUrlAction);
    menu->addAction(m_copyContactUrlAction);
    menu->addSeparator();
    menu->addAction(m_findChannelAction);
    if (m_mainWindow->menuBar()->isHidden()) {
        menu->addSeparator();
        menu->addMenu(settingsMenu(menu));
        // menu->addMenu(helpMenu(menu));
        menu->addSeparator();
        menu->addAction(m_quitAction);
    }
    return menu;
}

QMenu *Actions::settingsMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu(tr("設定(&S)"), parent);
#ifndef Q_WS_MAC
    menu->addAction(m_showMenuBarAction);
    menu->addAction(m_showToolBarAction);
#endif
    menu->addAction(m_showStatusBarAction);
    menu->addAction(m_showTabBarAction);
#ifndef Q_WS_MAC
    menu->addSeparator();
#endif
    menu->addAction(m_showSettingsAction);
    return menu;
}

QMenu *Actions::helpMenu(QWidget *parent) const
{
    QMenu *menu = new QMenu(tr("ヘルプ(&H)"), parent);
    menu->addAction(m_aboutQPeerCastYPAction);
    menu->addAction(m_aboutQtAction);
    return menu;
}

void Actions::playChannel(Channel *channel)
{
    if (!channel or !channel->isPlayable()) {
        m_mainWindow->showErrorMessage(tr("このチャンネルは再生できません。"));
        return;
    }
    QString player;
    QStringList args;
    Settings *s = qApp->settings();
    QString videoTypes = s->value("Player/VideoTypes").toString();
    QString soundTypes = s->value("Player/SoundTypes").toString();
    if (channel->type().contains(QRegExp(videoTypes, Qt::CaseInsensitive))) {
        player = s->value("Player/VideoPlayer").toString();
        args = Utils::shellwords(s->value("Player/VideoPlayerArgs").toString());
    } else if (channel->type().contains(QRegExp(soundTypes, Qt::CaseInsensitive))) {
        player = s->value("Player/SoundPlayer").toString();
        args = Utils::shellwords(s->value("Player/SoundPlayerArgs").toString());
    }
    if (player.isEmpty()) {
        m_mainWindow->showErrorMessage(
                tr("%1 用のプレイヤが設定されていません。").arg(channel->type().toUpper()));
        return;
    }
    startProcess(player, args, channel);
}

void Actions::setClipboardText(const QString &text)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text, QClipboard::Selection);
    clipboard->setText(text, QClipboard::Clipboard);
}

void Actions::openUrl(const QUrl &url)
{
    if (qApp->settings()->value("Program/UseCommonWebBrowser", true).toBool()) {
        QDesktopServices::openUrl(url);
    } else {
        QString browser = qApp->settings()->value("Program/WebBrowser").toString();
        startProcess(browser, QStringList(url.toString()));
    }
}

void Actions::startProcess(const QString &program, const QStringList &args, Channel *channel)
{
    if (!Process::start(program, args, channel))
        QMessageBox::warning(QApplication::activeWindow(),
                tr("エラー"), tr("プログラムの実行に失敗しました。"));
}

QAction *Actions::quitAction() const
{
    return m_quitAction;
}

QAction *Actions::updateYellowPageAction() const
{
    return m_updateYellowPageAction;
}

void Actions::updateYellowPageActionChanged()
{
    bool enabled = m_updateYellowPageAction->isEnabled();
    m_updateYellowPageToolBarAction->setEnabled(enabled);
    m_toggleAutoUpdateAction->setEnabled(enabled);
    if (!enabled)
        m_mainWindow->setAutoUpdateEnabled(false);
}

QAction *Actions::updateYellowPageToolBarAction() const
{
    return m_updateYellowPageToolBarAction;
}

QAction *Actions::toggleAutoUpdateAction() const
{
    return m_toggleAutoUpdateAction;
}

QAction *Actions::playChannelAction() const
{
    return m_playChannelAction;
}

QAction *Actions::addToFavoritesAction() const
{
    return m_addToFavoritesAction;
}

QAction *Actions::openContactUrlAction() const
{
    return m_openContactUrlAction;
}

QAction *Actions::openContactUrlWith2chBrowserAction() const
{
    return m_openContactUrlWith2chBrowserAction;
}

QAction *Actions::copyStreamUrlAction() const
{
    return m_copyStreamUrlAction;
}

QAction *Actions::copyContactUrlAction() const
{
    return m_copyContactUrlAction;
}

QAction *Actions::copyChannelInfoAction() const
{
    return m_copyChannelInfoAction;
}

QAction *Actions::findChannelAction() const
{
    return m_findChannelAction;
}

QAction *Actions::showStatusBarAction() const
{
    return m_showStatusBarAction;
}

QAction *Actions::showMenuBarAction() const
{
    return m_showMenuBarAction;
}

QAction *Actions::showToolBarAction() const
{
    return m_showToolBarAction;
}

QAction *Actions::showTabBarAction() const
{
    return m_showTabBarAction;
}

QAction *Actions::showSettingsAction() const
{
    return m_showSettingsAction;
}

QAction *Actions::aboutQPeerCastYPAction() const
{
    return m_aboutQPeerCastYPAction;
}

QAction *Actions::aboutQtAction() const
{
    return m_aboutQtAction;
}

