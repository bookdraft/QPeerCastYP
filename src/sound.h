/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef SOUND_H
#define SOUND_H

#include <QtCore>

class Sound
{
public:
    static void play(const QString &filename);
};

#endif // SOUND_H
