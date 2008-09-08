/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "utils.h"

namespace Utils
{
    QString unescape(const QString &str)
    {
        QString s = str;
        s.replace("&gt;", ">");
        s.replace("&lt;", "<");
        s.replace("&quot;", "\"");
        s.replace("&#039;", "'");
        s.replace("&amp;", "&");
        return s;
    }

    QString wsString()
    {
        QString ws = "Unknown";
#ifdef Q_WS_X11
        ws = "X11";
#endif
#ifdef Q_WS_MAC
        ws = "Macintosh";
#endif
#ifdef Q_WS_QWS
        ws = "QWS";
#endif
#ifdef Q_WS_WIN
        ws = "Windows";
#endif
        return ws;
    }

    QString osString()
    {
        QString os;
#ifdef Q_OS_AIX
        if (os.isEmpty()) os = "AIX";
#endif
#ifdef Q_OS_BSD4
        if (os.isEmpty()) os = "BSD4";
#endif
#ifdef Q_OS_BSDI
        if (os.isEmpty()) os = "BSDI";
#endif
#ifdef Q_OS_CYGWIN
        if (os.isEmpty()) os = "CYGWIN";
#endif
#ifdef Q_OS_DARWIN
        if (os.isEmpty()) os = "Darwin";
#endif
#ifdef Q_OS_DGUX
        if (os.isEmpty()) os = "DGUX";
#endif
#ifdef Q_OS_DYNIX
        if (os.isEmpty()) os = "DYNIX";
#endif
#ifdef Q_OS_FREEBSD
        if (os.isEmpty()) os = "FreeBSD";
#endif
#ifdef Q_OS_HPUX
        if (os.isEmpty()) os = "HPUX";
#endif
#ifdef Q_OS_HURD
        if (os.isEmpty()) os = "HURD";
#endif
#ifdef Q_OS_IRIX
        if (os.isEmpty()) os = "IRIX";
#endif
#ifdef Q_OS_LINUX
        if (os.isEmpty()) os = "Linux";
#endif
#ifdef Q_OS_LYNX
        if (os.isEmpty()) os = "LYNX";
#endif
#ifdef Q_OS_MSDOS
        if (os.isEmpty()) os = "MSDOS";
#endif
#ifdef Q_OS_NETBSD
        if (os.isEmpty()) os = "NETBSD";
#endif
#ifdef Q_OS_OS2
        if (os.isEmpty()) os = "OS2";
#endif
#ifdef Q_OS_OPENBSD
        if (os.isEmpty()) os = "OpenBSD";
#endif
#ifdef Q_OS_OS2EMX
        if (os.isEmpty()) os = "OS2EMX";
#endif
#ifdef Q_OS_OSF
        if (os.isEmpty()) os = "OSF";
#endif
#ifdef Q_OS_QNX6
        if (os.isEmpty()) os = "QNX6";
#endif
#ifdef Q_OS_QNX
        if (os.isEmpty()) os = "QNX";
#endif
#ifdef Q_OS_RELIANT
        if (os.isEmpty()) os = "RELIANT";
#endif
#ifdef Q_OS_SCO
        if (os.isEmpty()) os = "SCO";
#endif
#ifdef Q_OS_SOLARIS
        if (os.isEmpty()) os = "Solaris";
#endif
#ifdef Q_OS_ULTRIX
        if (os.isEmpty()) os = "ULTRIX";
#endif
#ifdef Q_OS_UNIX
        if (os.isEmpty()) os = "Unix";
#endif
#ifdef Q_OS_UNIXWARE
        if (os.isEmpty()) os = "UNIXWARE";
#endif
#ifdef Q_OS_WIN32
        if (os.isEmpty()) os = "Win32";
#endif
        if (os.isEmpty()) os = "Unknown";
        return os;
    }
}

