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
    setDefualtValues();
}

Settings::Settings(const QString &fileName, Format format, QObject *parent)
    : QSettings(fileName, format, parent), m_defaultSettings(0)
{
}

Settings::~Settings()
{
}

Settings *Settings::defaultSettings() const
{
    return m_defaultSettings;
}

void Settings::setDefualtValues()
{
    foreach (QString key, m_defaultSettings->allKeys())
        if (!contains(key))
            setValue(key, m_defaultSettings->value(key));
}

void Settings::reset()
{
    if (!m_defaultSettings)
        return;
    foreach (QString key, m_defaultSettings->allKeys())
        setValue(key, m_defaultSettings->value(key));
}

void Settings::copy(const QString &from, const QString &to)
{
    setValue(to, value(from));
}

void Settings::rename(const QString &from, const QString &to)
{
    copy(from, to);
    if (childKeys().isEmpty())
        remove(from);
}

