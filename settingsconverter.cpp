/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "settingsconverter.h"
#include "settings.h"

void SettingsConverter::convert(Settings *settings)
{
    qDebug() << "Version:" << settings->value("General/Version").toString();
    if (settings->value("General/Version").toString().isEmpty())
        convertV020ToV030(settings);
    settings->sync();
}

void SettingsConverter::convertV020ToV030(Settings *settings)
{
    qDebug() << "convertV020ToV030";

    // イエローページ
    for (int i = 0; i < 100; ++i) {
        QString key1 = QString("YellowPage/Item%1/").arg(i, 2, 10, QChar('0'));
        QString key2 = QString("YellowPage/Items/%1/").arg(i + 1);
        if (!settings->contains(key1))
            break;
        settings->setValue("YellowPage/Items/size", i + 1);
        settings->copy(key1, key2 + "Enabled");
        settings->copy(key1 + "Name", key2 + "Name");
        settings->copy(key1 + "Type", key2 + "Type");
        settings->copy(key1 + "Url", key2 + "Url");
        settings->copy(key1 + "NameSpaces", key2 + "NameSpaces");
        settings->copy(key1 + "UsePCRawProxy", key2 + "UsePCRawProxy");
        settings->remove(key1);
    }

    // お気に入り
    for (int i = 0; i < 100; ++i) {
        QString key1 = QString("Favorite/Item%1/").arg(i, 2, 10, QChar('0'));
        QString key2 = QString("Favorite/Items/%1/").arg(i + 1);
        if (!settings->contains(key1))
            break;
        settings->setValue("Favorite/Items/size", i + 1);
        settings->copy(key1, key2 + "Enabled");
        settings->copy(key1 + "Pattern", key2 + "Pattern");
        settings->copy(key1 + "MatchFlags", key2 + "MatchFlags");
        settings->copy(key1 + "TargetFlags", key2 + "TargetFlags");
        settings->copy(key1 + "Point", key2 + "Point");
        settings->remove(key1);
    }

    settings->setValue("General/Version", "0.3.0");
}

