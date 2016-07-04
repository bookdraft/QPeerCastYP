/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef SETTINGSCONVERTER_H
#define SETTINGSCONVERTER_H

class Settings;

class SettingsConverter
{
public:
    static void convert(Settings *settings);
    static void convertToV030(Settings *settings);
    static void convertToV050(Settings *settings);
};

#endif // SETTINGSCONVERTER_H
