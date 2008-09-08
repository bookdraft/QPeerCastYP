/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "yellowpagemanager.h"
#include "settings.h"

YellowPageManager::YellowPageManager(Settings *settings)
    : YellowPage(QString("http://%1/YellowPageManager").arg(qApp->applicationName()), Manager),
      m_settings(settings)
{
    m_lastUpdatedTime = QDateTime::currentDateTime();
    setName(tr("All"));
    m_yellowPages.clear();
    loadYellowPages();
}

YellowPageManager::~YellowPageManager()
{
    clear();
}

void YellowPageManager::addYellowPage(YellowPage *yellowPage)
{
    if (!yellowPage)
        return;
    m_yellowPages += yellowPage;
    connect(yellowPage, SIGNAL(updateDone(YellowPage *, bool)),
            this, SIGNAL(updateDone(YellowPage *, bool)));
}

YellowPage *YellowPageManager::yellowPage(const QString &url)
{
    foreach (YellowPage *yp, m_yellowPages)
        if (yp->url() == url)
            return yp;

    return 0;
}

void YellowPageManager::update()
{
    foreach (YellowPage *yp, m_yellowPages)
        if (yp->isEnabled())
            yp->update();
}

void YellowPageManager::stopUpdate()
{
    foreach (YellowPage *yp, m_yellowPages)
        if (yp->isEnabled())
            yp->stopUpdate();
}

QDateTime YellowPageManager::lastUpdatedTime()
{
    foreach (YellowPage *yp, m_yellowPages) {
        if (!yp->isEnabled())
            continue;
        if (m_lastUpdatedTime < yp->lastUpdatedTime())
            m_lastUpdatedTime = yp->lastUpdatedTime();
    }
    return m_lastUpdatedTime;
}

bool YellowPageManager::isUpdating()
{
    foreach (YellowPage *yp, m_yellowPages)
        if (yp->isUpdating())
            return true;
    return false;
}

ChannelList &YellowPageManager::channels()
{
    m_channels.clear();
    foreach (YellowPage *yp, m_yellowPages)
        if (yp->isEnabled())
            m_channels += yp->channels();

    return m_channels;
}

ChannelList &YellowPageManager::channels(const QString &ypName)
{
    foreach (YellowPage *yp, m_yellowPages)
        if (yp->name() == ypName)
            return yp->channels();
    m_channels.clear();
    return m_channels;
}

void YellowPageManager::clear()
{
    while (!m_yellowPages.isEmpty())
        delete m_yellowPages.takeFirst();
}

void YellowPageManager::saveYellowPages()
{
    if (!m_settings)
        return;
    m_settings->remove("YellowPage");
    for (int i = 0; i < m_yellowPages.count(); ++i) {
        QString key = QString("YellowPage/Item%1").arg(i, 2, 10, QChar('0'));
        YellowPage *yp = m_yellowPages[i];
        m_settings->setValue(key, yp->isEnabled());
        m_settings->setValue(key + "/Name", yp->name());
        m_settings->setValue(key + "/Url", yp->url().toString());
        m_settings->setValue(key + "/NameSpaces", yp->nameSpaces());
        m_settings->setValue(key + "/UsePCRawProxy", yp->usePCRawProxy());
    }
}

void YellowPageManager::loadYellowPages()
{
    if (!m_settings)
        return;
    bool excludeDefault = m_settings->childGroups().contains("YellowPage");
    for (int i = 0; i < 100; ++i) {
        QString key = QString("YellowPage/Item%1").arg(i, 2, 10, QChar('0'));
        if (!m_settings->contains(key, excludeDefault))
            break;
        YellowPage *yp = new YellowPage(m_settings->value(key + "/Url").toString());
        yp->setEnabled(m_settings->value(key).toBool());
        yp->setName(m_settings->value(key + "/Name").toString());
        yp->setNameSpaces(m_settings->value(key + "/NameSpaces").toStringList());
        yp->setUsePCRawProxy(m_settings->value(key + "/UsePCRawProxy").toBool());
        addYellowPage(yp);
    }
}

