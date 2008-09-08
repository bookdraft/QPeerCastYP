/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "settings.h"
#include "yellowpage.h"
#include "yellowpagemanager.h"

Settings::Settings(Format format, Scope scope, const QString &org, const QString &appName,
                   Settings *defaultSettings, QObject *parent)
    : QSettings(format, scope, org, appName, parent), m_defaultSettings(defaultSettings)
{
}

Settings::Settings(const QString &fileName, Format format, QObject *parent)
    : QSettings(fileName, format, parent), m_defaultSettings(0)
{
}

Settings::~Settings()
{
}

bool Settings::contains(const QString &key, bool excludeDefault) const
{
    if (QSettings::contains(key))
        return true;
    else if (!excludeDefault and hasDefaultSettings()
            and m_defaultSettings->contains(key))
        return true;
    return false;
}

Settings *Settings::defaultSettings() const
{
    return m_defaultSettings;
}

bool Settings::hasDefaultSettings() const
{
    return (bool)m_defaultSettings;
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue)
{
    QVariant value = defaultValue;
    if (value.isNull() and hasDefaultSettings())
        value = m_defaultSettings->value(key);
    return QSettings::value(key, value);
}

