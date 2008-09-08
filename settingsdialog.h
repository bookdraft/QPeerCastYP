/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui>

class Settings;
class ChannelMatcher;
class GeneralWidget;
class YellowPageEdit;
class FavoriteEdit;
class PlayerEdit;
class NotificationWidget;
class NetworkWidget;
class AdvancedWidget;

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    enum WidgetIndex { General, YellowPage, Favorite, Notification, Player, Network, Advanced };
    SettingsDialog(Settings *settings, QWidget *parent = 0);
    virtual ~SettingsDialog();

    GeneralWidget *generalWidget() const;
    YellowPageEdit *yellowPageEdit() const;
    FavoriteEdit *favoriteEdit() const;
    NotificationWidget *notificationWidget() const;
    NetworkWidget *networkWidget() const;

    void setCurrentWidget(WidgetIndex index);

public slots:
    void accept();

private slots:
    void clicked(QAbstractButton *button);

private:
    QTabWidget *m_tabWidget;
    QDialogButtonBox *m_buttonBox;
    Settings *m_settings;
    GeneralWidget *m_generalWidget;
    YellowPageEdit *m_yellowPageEdit;
    FavoriteEdit *m_favoriteEdit;
    PlayerEdit *m_playerEdit;
    NotificationWidget *m_notificationWidget;
    NetworkWidget *m_networkWidget;
    AdvancedWidget *m_advancedWidget;
};

#endif // SETTINGSDIALOG_H
