/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

class Actions;
class ChannelListWidget;
class ChannelListTabWidget;
class ChannelListFindBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWindow();

    void setup();

    Actions *actions() const;
    ChannelListWidget *currentChannelListWidget() const;
    ChannelListTabWidget *channelListTabWidget() const;

public slots:
    void setVisible(bool visible);
    void quit();

    void updateYellowPage();
    void setAutoUpdateEnabled(bool enabled);
    void setAutoUpdateInterval(int sec);
    void playChannel();
    void addToFavorites();
    void openContactUrl();
    void copyStreamUrl();
    void copyContactUrl();
    void copyChannelInfo();
    void findChannel();

    void showErrorMessage(const QString &message);
    void updateStatusBar(const QString &message = QString());

    void setStatusBarVisible(bool shown);
    void setMenuBarVisible(bool shown);
    void setToolBarVisible(bool shown);
    void setTabBarVisible(bool shown);
    void showSettings();

    void aboutQt();
    void aboutQPeerCastYP();

private slots:
    void toolBarOrientationChanged(Qt::Orientation orientation);

private:
    void setupActions();
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void setupChannelListWidget();
    void clearChannelListWidget();

    void closeEvent(QCloseEvent *event);

    void readSettings();
    void writeSettings();

    QPoint m_pos;
    QToolBar *m_mainToolBar;
    Actions *m_actions;
    ChannelListTabWidget *m_channelListTabWidget;
    ChannelListFindBar *m_channelListFindBar;
    bool m_autoUpdateEnabled;
    QTimer m_autoUpdateTimer;
};

#endif // MAINWINDOW_H
