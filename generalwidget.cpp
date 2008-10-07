/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "generalwidget.h"
#include "channellistwidget.h"
#include "settings.h"

GeneralWidget::GeneralWidget(Settings *settings, QWidget *parent)
    : SettingWidget(parent), m_settings(settings)
{
    setupUi(this);
    setDirty();
    QCompleter *completer = new QCompleter(this);
    completer->setModel(new QDirModel(completer));
    webBrowserEdit->setCompleter(completer);

    linkTypeComboBox->addItem(tr("チャンネルを再生"), (int)ChannelListWidget::ChannelLink);
    linkTypeComboBox->addItem(tr("コンタクトURLを開く"), (int)ChannelListWidget::ContactLink);
}

GeneralWidget::~GeneralWidget()
{
}

void GeneralWidget::setValue(bool reset)
{
    Settings *settings = reset ? m_settings->defaultSettings() : m_settings;
    updateAtStartupCheckBox->setChecked(
            settings->value("AtStartup/UpdateYellowPage").toBool());
    dontShowMinusScoreChannelCheckBox->setChecked(
            settings->value("General/DontShowMinusScoreChannels").toBool());
    autoUpdateCheckBox->setChecked(
            settings->value("General/AutoUpdate").toBool());
    autoUpdateIntervalSpinBox->setValue(
            settings->value("General/AutoUpdateInterval").toInt() / 60);
    customToolTipCheckBox->setChecked(
            settings->value("ChannelListWidget/CustomToolTip").toBool());
    linkEnabledCheckBox->setChecked(
            settings->value("ChannelListWidget/LinkEnabled").toBool());
    linkTypeComboBox->setCurrentIndex(linkTypeComboBox->findData(
            settings->value("ChannelListWidget/LinkType").toBool()));
    showSystemTrayIconCheckBox->setChecked(
            settings->value("SystemTrayIcon/Enabled").toBool());
    bool useCommonWebBrowser = settings->value("Program/UseCommonWebBrowser").toBool();
    useCommonWebBrowserRadioButton->setChecked(useCommonWebBrowser);
    useCustomWebBrowserRadioButton->setChecked(!useCommonWebBrowser);
    webBrowserEdit->setText(settings->value("Program/WebBrowser").toString());
}

void GeneralWidget::write()
{
    m_settings->setValue("AtStartup/UpdateYellowPage",
            updateAtStartupCheckBox->isChecked());
    m_settings->setValue("General/DontShowMinusScoreChannels",
            dontShowMinusScoreChannelCheckBox->isChecked());
    m_settings->setValue("General/AutoUpdate",
            autoUpdateCheckBox->isChecked());
    m_settings->setValue("General/AutoUpdateInterval",
            autoUpdateIntervalSpinBox->value() * 60);
    m_settings->setValue("ChannelListWidget/CustomToolTip",
            customToolTipCheckBox->isChecked());
    m_settings->setValue("ChannelListWidget/LinkEnabled",
            linkEnabledCheckBox->isChecked());
    m_settings->setValue("ChannelListWidget/LinkType",
            linkTypeComboBox->itemData(linkTypeComboBox->currentIndex()));
    m_settings->setValue("SystemTrayIcon/Enabled",
            showSystemTrayIconCheckBox->isChecked());
    m_settings->setValue("Program/UseCommonWebBrowser",
            useCommonWebBrowserRadioButton->isChecked());
    m_settings->setValue("Program/WebBrowser", webBrowserEdit->text());
}

void GeneralWidget::on_selectWebBrowserButton_clicked()
{
    QString dir;
    if (!webBrowserEdit->text().isEmpty())
        dir = QFileInfo(webBrowserEdit->text()).dir().canonicalPath();
    QString program = QFileDialog::getOpenFileName(this, tr("プログラムを選択"));
    if (!program.isEmpty())
        webBrowserEdit->setText(program);
}

