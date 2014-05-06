/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtGui>

class MainWindow;
class SystemTrayIcon;
class Actions;
class YellowPageManager;
class Settings;
class PCRawProxy;

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<Application *>(QApplication::instance()))

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char *argv[]);
    virtual ~Application();

    MainWindow *mainWindow() const;
    SystemTrayIcon *systemTrayIcon() const;
    Actions *actions() const;
    YellowPageManager *yellowPageManager() const;
    QProcess *peercast() const;
    PCRawProxy *pcrawProxy() const;
    Settings *settings() const;
    QIcon applicationIcon() const;

    static Application *instance();

private:
    MainWindow *m_mainWindow;
    SystemTrayIcon *m_systemTrayIcon;
    Settings *m_settings;
    YellowPageManager *m_yellowPageManager;
    QProcess *m_peercast;
    PCRawProxy *m_pcrawProxy;
};

#endif // APPLICATION_H
