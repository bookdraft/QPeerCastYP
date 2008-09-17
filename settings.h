/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore>

class YellowPage;
class YellowPageManager;

class Settings : public QSettings
{
public:
    Settings(Format format, Scope, const QString &org, const QString &appName,
             Settings *defaultSettings = 0, QObject *parent = 0);
    Settings(const QString &fileName, Format format, QObject *parent = 0);
    virtual ~Settings();

    Settings *defaultSettings() const;
    void reset();
    void copy(const QString &from, const QString &to);
    void rename(const QString &from, const QString &to);

private:
    void setDefualtValues();

    Settings *m_defaultSettings;
};

#endif // SETTINGS_H
