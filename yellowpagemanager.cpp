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
    : YellowPage(QString("http://%1/YellowPageManager").arg(qApp->applicationName())),
      m_settings(settings)
{
    m_lastUpdatedTime = QDateTime::currentDateTime();
    setName(tr("All"));
    m_isManager = true;
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

ChannelList YellowPageManager::channels() const
{
    ChannelList channels;
    foreach (YellowPage *yp, m_yellowPages)
        if (yp->isEnabled())
            channels += yp->channels();

    return channels;
}

ChannelList YellowPageManager::channels(const QString &ypName) const
{
    foreach (YellowPage *yp, m_yellowPages)
        if (yp->name() == ypName)
            return yp->channels();
    return ChannelList();
}

void YellowPageManager::clear()
{
    while (!m_yellowPages.isEmpty())
        delete m_yellowPages.takeFirst();
}

void YellowPageManager::loadYellowPages()
{
    if (!m_settings)
        return;
    int size = m_settings->beginReadArray("YellowPage/Items");
    for (int i = 0; i < size; ++i) {
        m_settings->setArrayIndex(i);
        YellowPage *yp = new YellowPage(m_settings->value("Url").toString());
        yp->setEnabled(m_settings->value("Enabled").toBool());
        yp->setName(m_settings->value("Name").toString());
        yp->setType((YellowPage::Type)m_settings->value("Type").toInt());
        yp->setNameSpaces(m_settings->value("NameSpaces").toStringList());
        addYellowPage(yp);
    }
    m_settings->endArray();
}

void YellowPageManager::saveYellowPages()
{
    if (!m_settings)
        return;
    m_settings->remove("YellowPage/Items");
    m_settings->beginWriteArray("YellowPage/Items");
    for (int i = 0; i < m_yellowPages.count(); ++i) {
        m_settings->setArrayIndex(i);
        YellowPage *yp = m_yellowPages[i];
        m_settings->setValue("Enabled", yp->isEnabled());
        m_settings->setValue("Name", yp->name());
        m_settings->setValue("Type", yp->type());
        m_settings->setValue("Url", yp->url().toString());
        m_settings->setValue("NameSpaces", yp->nameSpaces());
    }
    m_settings->endArray();
}

