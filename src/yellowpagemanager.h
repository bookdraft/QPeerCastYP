/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef YELLOWPAGEMANAGER_H
#define YELLOWPAGEMANAGER_H

#include <QtCore>

#include "yellowpage.h"
#include "channel.h"

class Channel;
class Settings;

class YellowPageManager : public YellowPage
{
    Q_OBJECT
public:
    YellowPageManager(Settings *settings = 0);
    virtual ~YellowPageManager();

    YellowPage *createYellowPage(const QString &url, YellowPage::Type type = YellowPage::IndexTxt);
    YellowPage *yellowPage(const QString &url);

    void addYellowPage(YellowPage *yellowPage);
    void clear();

    void update();
    void stopUpdate();
    bool isUpdating();

    virtual QDateTime lastUpdatedTime();

    ChannelList channels() const;
    ChannelList channels(const QString &ypName) const;

    void saveYellowPages();
    void loadYellowPages();

signals:
    void updateDone(YellowPage *yellowPage, bool error);

private:
    Settings *m_settings;
};

#endif // YELLOWPAGEMANAGER_H
