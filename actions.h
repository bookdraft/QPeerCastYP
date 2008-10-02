/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef ACTIONS_H
#define ACTIONS_H

#include <QtGui>

class MainWindow;
class Channel;

class Actions : public QObject
{
    Q_OBJECT
public:
    Actions(MainWindow *);
    virtual ~Actions();

    QMenu *fileMenu(QWidget *parent = 0) const;
    QAction *quitAction() const;

    QMenu *yellowPageMenu(QWidget *parent = 0) const;
    QAction *updateYellowPageAction() const;
    QAction *updateYellowPageToolBarAction() const;
    QAction *toggleAutoUpdateAction() const;
    QAction *playChannelAction() const;
    QAction *addToFavoritesAction() const;
    QAction *openContactUrlAction() const;
    QAction *openContactUrlWith2chBrowserAction() const;
    QAction *copyStreamUrlAction() const;
    QAction *copyContactUrlAction() const;
    QAction *copyChannelInfoAction() const;
    QAction *findChannelAction() const;

    QMenu *settingsMenu(QWidget *parent = 0) const;
    QAction *showStatusBarAction() const;
    QAction *showMenuBarAction() const;
    QAction *showToolBarAction() const;
    QAction *showTabBarAction() const;
    QAction *showSettingsAction() const;

    QMenu *helpMenu(QWidget *parent = 0) const;
    QAction *aboutQPeerCastYPAction() const;
    QAction *aboutQtAction() const;

    void loadUserActions();

public slots:
    void playChannel(Channel *channel);
    void setClipboardText(const QString &text);
    void openUrl(const QUrl &url);

private slots:
    void updateYellowPageActionChanged();

private:
    MainWindow *m_mainWindow;
    QAction *m_quitAction;

    QAction *m_updateYellowPageAction;
    QAction *m_updateYellowPageToolBarAction;
    QAction *m_toggleAutoUpdateAction;
    QAction *m_playChannelAction;
    QAction *m_addToFavoritesAction;
    QAction *m_openContactUrlAction;
    QAction *m_openContactUrlWith2chBrowserAction;
    QAction *m_copyStreamUrlAction;
    QAction *m_copyContactUrlAction;
    QAction *m_copyChannelInfoAction;
    QAction *m_findChannelAction;

    QAction *m_showMenuBarAction;
    QAction *m_showToolBarAction;
    QAction *m_showStatusBarAction;
    QAction *m_showTabBarAction;
    QAction *m_showSettingsAction;

    QAction *m_aboutQPeerCastYPAction;
    QAction *m_aboutQtAction;

    QList<QAction *> m_userActions;
};

#endif // ACTIONS_H
