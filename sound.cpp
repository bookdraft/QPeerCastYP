/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "sound.h"

void Sound::play(const QString &filename)
{
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
    QSound::play(filename);
#elif defined(Q_OS_UNIX)
    QString player;
    if (filename.endsWith(".wav"))
        player = "aplay";
    else if (filename.endsWith(".ogg"))
        player = "ogg123";
    else if (filename.endsWith(".mp3"))
        player = "mpg123";
    if (!player.isEmpty())
        QProcess::startDetached(player, QStringList() << filename);
#endif
}

