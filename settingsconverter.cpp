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
    if (settings->value("General/Version").toString().isEmpty())
        convertV020ToV030(settings);
    settings->sync();
}

void SettingsConverter::convertV020ToV030(Settings *settings)
{
    qDebug() << "convertV020ToV030";
    for (int i = 0; i < 100; ++i) {
        QString key1 = QString("Favorite/Item%1").arg(i, 2, 10, QChar('0'));
        QString key2 = QString("Favorite/Item%1").arg(i, 3, 10, QChar('0'));
        if (!settings->contains(key1))
            continue;
        settings->setValue(key2, settings->value(key1));
        settings->setValue(key2 + "/Pattern", settings->value(key1 + "/Pattern"));
        settings->setValue(key2 + "/MatchFlags", settings->value(key1 + "/MatchFlags"));
        settings->setValue(key2 + "/TargetFlags", settings->value(key1 + "/TargetFlags"));
        settings->setValue(key2 + "/Point", settings->value(key1 + "/Point"));
        settings->remove(key1);
    }
    settings->setValue("General/Version", "0.3.0");
}

