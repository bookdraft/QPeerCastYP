/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "application.h"
#include "mainwindow.h"
#include "actions.h"
#include "systemtrayicon.h"
#include "yellowpagemanager.h"
#include "pcrawproxy.h"
#include "settings.h"
#include "settingsdialog.h"
#include "settingsconverter.h"
#include "network.h"
#include "utils.h"

Application::Application(int &argc, char *argv[])
    : QApplication(argc, argv)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);

    QTranslator *qtTr = new QTranslator(this);
    QTranslator *appTr = new QTranslator(this);
    QStringList trDirs;
    trDirs += QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    trDirs += ":/translations/";
    QString locale = QLocale::system().name().toLower();
    foreach (QString dir, trDirs)
        if (qtTr->load("qt_" + locale, dir))
            break;
    installTranslator(qtTr);
    foreach (QString dir, trDirs) {
#ifdef Q_WS_MAC
        if (appTr->load("qpeercastyp_osx_" + locale, dir))
#else
        if (appTr->load("qpeercastyp_" + locale, dir))
#endif
            break;
    }
    installTranslator(appTr);

    QString appName = "QPeerCastYP";
    setApplicationName(appName);
    setOrganizationName(appName);

#ifndef Q_WS_MAC
    setWindowIcon(applicationIcon());
#endif

    Network::setUserAgent(QString("Mozilla/4.0 (%1/%2; %3; %4)")
            .arg(appName).arg(VERSION).arg(Utils::wsString()).arg(Utils::osString()));

    Settings *defaultSettings = new Settings(":/default.conf", QSettings::IniFormat);
#ifdef Q_WS_WIN
    m_settings = new Settings(QSettings::IniFormat, QSettings::UserScope,
                              organizationName(), applicationName(), defaultSettings);
#else
    m_settings = new Settings(QSettings::NativeFormat, QSettings::UserScope,
                              organizationName(), applicationName(), defaultSettings);
#endif // Q_WS_WIN

    bool firstRun = m_settings->value("General/FirstRun").toBool();
    if (firstRun) {
        m_settings->setValue("General/FirstRun", false);
#ifdef Q_WS_X11
        m_settings->setValue("Player/VideoPlayer",
                defaultSettings->value("Player/Linux/VideoPlayer"));
        m_settings->setValue("Player/VideoPlayerArgs",
                defaultSettings->value("Player/Linux/VideoPlayerArgs"));
        m_settings->setValue("Player/SoundPlayer",
                defaultSettings->value("Player/Linux/SoundPlayer"));
        m_settings->setValue("Player/SoundPlayerArgs",
                defaultSettings->value("Player/Linux/SoundPlayerArgs"));
#endif // Q_WS_X11
#ifdef Q_WS_WIN
        m_settings->setValue("Player/VideoPlayer",
                defaultSettings->value("Player/Windows/VideoPlayer"));
        m_settings->setValue("Player/VideoPlayerArgs",
                defaultSettings->value("Player/Windows/VideoPlayerArgs"));
        m_settings->setValue("Player/SoundPlayer",
                defaultSettings->value("Player/Windows/SoundPlayer"));
        m_settings->setValue("Player/SoundPlayerArgs",
                defaultSettings->value("Player/Windows/SoundPlayerArgs"));
#endif // Q_WS_WIN
    }

    SettingsConverter::convert(m_settings);
    m_settings->setValue("General/Version", VERSION);

    m_yellowPageManager = new YellowPageManager(m_settings);

    m_mainWindow = new MainWindow();
    m_mainWindow->setup();
    m_mainWindow->show();
    m_mainWindow->setUnifiedTitleAndToolBarOnMac(true);

    m_systemTrayIcon = new SystemTrayIcon(this);
    if (m_settings->value("SystemTrayIcon/Enabled").toBool())
        m_systemTrayIcon->show();

    m_peercast = new QProcess(this);
    if (m_settings->value("AtStartup/RunPeerCast").toBool()) {
        QString program = m_settings->value("Program/PeerCast").toString();
        QStringList args = Utils::shellwords(m_settings->value("Program/PeerCastArgs").toString());
        if (!program.isEmpty()) {
            m_peercast->setWorkingDirectory(QFileInfo(program).dir().absolutePath());
            m_peercast->start(program, args);
            if (!m_peercast->waitForStarted())
                QMessageBox::warning(m_mainWindow, tr("エラー"), tr("PeerCast の起動に失敗しました。"));
        } else {
            qWarning() << tr("PeerCast プログラムが設定されていません。");
        }
    }

    QString pcrawProgram = m_settings->value("Program/PCRawProxy").toString();
    int pcrawPort = QUrl(m_settings->value("PCRaw/ProxyServerUrl").toString()).port();
    QUrl serverUrl = m_settings->value("PeerCast/ServerUrl").toString();
    m_pcrawProxy = new PCRawProxy(pcrawProgram, pcrawPort, serverUrl);
    if (m_settings->value("AtStartup/RunPCRawProxy").toBool())
        m_pcrawProxy->start();

    if (m_settings->value("AtStartup/UpdateYellowPage").toBool())
        QTimer::singleShot(1 * 1000, m_mainWindow->actions()->updateYellowPageAction(), SLOT(trigger()));
}

Application::~Application()
{
    delete m_mainWindow;
    delete m_systemTrayIcon;
    delete m_settings;
}

MainWindow *Application::mainWindow() const
{
    return m_mainWindow;
}

SystemTrayIcon *Application::systemTrayIcon() const
{
    return m_systemTrayIcon;
}

Actions *Application::actions() const
{
    return m_mainWindow->actions();
}

YellowPageManager *Application::yellowPageManager() const
{
    return m_yellowPageManager;
}

QProcess *Application::peercast() const
{
    return m_peercast;
}

PCRawProxy *Application::pcrawProxy() const
{
    return m_pcrawProxy;
}

Settings *Application::settings() const
{
    return m_settings;
}

QIcon Application::applicationIcon() const
{
    return QIcon(":/images/qpeercastyp.png");
}

Application *Application::instance()
{
    return qobject_cast<Application *>(QApplication::instance());
}

