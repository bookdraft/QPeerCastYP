/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "networkwidget.h"
#include "settings.h"

NetworkWidget::NetworkWidget(Settings *settings, QWidget *parent)
    : SettingWidget(parent), m_settings(settings)
{
    setupUi(this);
    setDirty();
}

NetworkWidget::~NetworkWidget()
{
}

void NetworkWidget::setValue(bool reset)
{
    Settings *settings = reset ? m_settings->defaultSettings() : m_settings;

    httpTimeoutIntervalSpinBox->setValue(settings->value("Network/TimeoutInterval").toInt());

    QUrl pcUrl(settings->value("PeerCast/ServerUrl").toString());
    peerCastServerHostEdit->setText(pcUrl.host());
    peerCastServerPortSpinBox->setValue(pcUrl.port());
    runPeerCastAtStartupCheckBox->setChecked(settings->value("AtStartup/RunPeerCast").toBool());
    peerCastProgramEdit->setText(settings->value("Program/PeerCast").toString());

    QUrl pcrUrl(settings->value("PCRaw/ProxyServerUrl").toString());
    pcrawProxyServerHostEdit->setText(pcrUrl.host());
    pcrawProxyServerPortSpinBox->setValue(pcrUrl.port());
    runPCRawProxyAtStartup->setChecked(settings->value("AtStartup/RunPCRawProxy").toBool());
    pcrawProxyProgramEdit->setText(settings->value("Program/PCRawProxy").toString());
}

void NetworkWidget::write()
{
    m_settings->setValue("Network/TimeoutInterval", httpTimeoutIntervalSpinBox->value());
    m_settings->setValue("PeerCast/ServerUrl", QString("http://%1:%2/")
            .arg(peerCastServerHostEdit->text()).arg(peerCastServerPortSpinBox->value()));
    m_settings->setValue("AtStartup/RunPeerCast", runPeerCastAtStartupCheckBox->isChecked());
    m_settings->setValue("Program/PeerCast", peerCastProgramEdit->text());
    m_settings->setValue("PCRaw/ProxyServerUrl", QString("http://%1:%2/")
            .arg(pcrawProxyServerHostEdit->text()).arg(pcrawProxyServerPortSpinBox->value()));
    m_settings->setValue("AtStartup/RunPCRawProxy", runPCRawProxyAtStartup->isChecked());
    m_settings->setValue("Program/PCRawProxy", pcrawProxyProgramEdit->text());
}

void NetworkWidget::on_selectPeerCastProgramButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("プログラムを選択"));
    if (!file.isEmpty())
        peerCastProgramEdit->setText(file);
}

void NetworkWidget::on_selectPCRawProxyProgramButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("プログラムを選択"));
    if (!file.isEmpty())
        pcrawProxyProgramEdit->setText(file);
}

