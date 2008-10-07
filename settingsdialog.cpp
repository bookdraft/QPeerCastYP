/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "settingsdialog.h"
#include "settings.h"
#include "generalwidget.h"
#include "yellowpageedit.h"
#include "favoriteedit.h"
#include "channelmatcher.h"
#include "playeredit.h"
#include "notificationwidget.h"
#include "networkwidget.h"
#include "advancedwidget.h"
#include "useractionedit.h"

SettingsDialog::SettingsDialog(Settings *settings, QWidget *parent)
    : QDialog(parent), m_settings(settings)
{
    m_tabWidget = new QTabWidget;

    m_generalWidget = new GeneralWidget(settings, m_tabWidget);
    m_generalWidget->setValue();
    m_tabWidget->insertTab(General, m_generalWidget, tr("全般"));

    m_yellowPageEdit = new YellowPageEdit(settings, m_tabWidget);
    m_yellowPageEdit->setValue();
    m_tabWidget->insertTab(YellowPage, m_yellowPageEdit, tr("イエローページ"));

    m_favoriteEdit = new FavoriteEdit(settings, m_tabWidget);
    m_favoriteEdit->setValue();
    m_tabWidget->insertTab(Favorite, m_favoriteEdit, tr("お気に入り"));

    m_notificationWidget = new NotificationWidget(settings, m_tabWidget);
    m_notificationWidget->setValue();
    m_tabWidget->insertTab(Notification, m_notificationWidget, tr("通知"));

    m_playerEdit = new PlayerEdit(settings, m_tabWidget);
    m_playerEdit->setValue();
    m_tabWidget->insertTab(Player, m_playerEdit, tr("プレイヤ"));

    m_userActionEdit = new UserActionEdit(settings, m_tabWidget);
    m_userActionEdit->setValue();
    m_tabWidget->insertTab(UserAction, m_userActionEdit, tr("アクション"));

    m_networkWidget = new NetworkWidget(settings, m_tabWidget);
    m_networkWidget->setValue();
    m_tabWidget->insertTab(Network, m_networkWidget, tr("ネットワーク"));

    // m_advancedWidget = new AdvancedWidget(settings, m_tabWidget);
    // m_advancedWidget->setValues();
    // m_tabWidget->insertTab(Advanced, m_advancedWidget, tr("詳細設定"));

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel
                                     | QDialogButtonBox::Reset);
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_buttonBox, SIGNAL(clicked(QAbstractButton *)),
            this, SLOT(clicked(QAbstractButton *)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_tabWidget);
    layout->addWidget(m_buttonBox);
    setLayout(layout);

    setWindowTitle(tr("%1 の設定").arg(qApp->applicationName()));
    QSize minSize = minimumSizeHint();
    resize(minSize.width() + 150, minSize.height());
    setModal(true);
    show();
}

SettingsDialog::~SettingsDialog()
{
}

GeneralWidget *SettingsDialog::generalWidget() const
{
    return m_generalWidget;
}

YellowPageEdit *SettingsDialog::yellowPageEdit() const
{
    return m_yellowPageEdit;
}

FavoriteEdit *SettingsDialog::favoriteEdit() const
{
    return m_favoriteEdit;
}

NotificationWidget *SettingsDialog::notificationWidget() const
{
    return m_notificationWidget;
}

UserActionEdit *SettingsDialog::userActionEdit() const
{
    return m_userActionEdit;
}

NetworkWidget *SettingsDialog::networkWidget() const
{
    return m_networkWidget;
}

void SettingsDialog::setCurrentWidget(WidgetIndex index)
{
    m_tabWidget->setCurrentIndex(index);
}

void SettingsDialog::accept()
{
    if (m_generalWidget->isDirty())
        m_generalWidget->write();
    if (m_yellowPageEdit->isDirty())
        m_yellowPageEdit->write();
    if (m_favoriteEdit->isDirty())
        m_favoriteEdit->write();
    if (m_notificationWidget->isDirty())
        m_notificationWidget->write();
    if (m_playerEdit->isDirty())
        m_playerEdit->write();
    if (m_userActionEdit->isDirty())
        m_userActionEdit->write();
    if (m_networkWidget->isDirty())
        m_networkWidget->write();
    m_settings->sync();
    QDialog::accept();
}

void SettingsDialog::clicked(QAbstractButton *button)
{
    if (m_buttonBox->standardButton(button) == QDialogButtonBox::Reset) {
        SettingWidget *widget = qobject_cast<SettingWidget *>(m_tabWidget->currentWidget());
        if (widget)
            widget->setValue(true);
    }
}

