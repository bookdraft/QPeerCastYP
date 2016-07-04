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
#if defined(Q_WS_MAC)
        ws = "Macintosh";
#elif defined(Q_WS_X11)
        ws = "X11";
#elif defined(Q_WS_WIN)
        ws = "Windows";
#endif
        return ws;
    }

    QString osString()
    {
        QString os;
#if defined(Q_OS_DARWIN)
        if (os.isEmpty()) os = "Darwin";
#elif defined(Q_OS_LINUX)
        if (os.isEmpty()) os = "Linux";
#elif defined(Q_OS_WIN32)
        if (os.isEmpty()) os = "Win32";
#elif defined(Q_OS_AIX)
        if (os.isEmpty()) os = "AIX";
#elif defined(Q_OS_BSD4)
        if (os.isEmpty()) os = "BSD4";
#elif defined(Q_OS_BSDI)
        if (os.isEmpty()) os = "BSDI";
#elif defined(Q_OS_CYGWIN)
        if (os.isEmpty()) os = "CYGWIN";
#elif defined(Q_OS_DGUX)
        if (os.isEmpty()) os = "DGUX";
#elif defined(Q_OS_DYNIX)
        if (os.isEmpty()) os = "DYNIX";
#elif defined(Q_OS_FREEBSD)
        if (os.isEmpty()) os = "FreeBSD";
#elif defined(Q_OS_HPUX)
        if (os.isEmpty()) os = "HPUX";
#elif defined(Q_OS_HURD)
        if (os.isEmpty()) os = "HURD";
#elif defined(Q_OS_IRIX)
        if (os.isEmpty()) os = "IRIX";
#elif defined(Q_OS_LYNX)
        if (os.isEmpty()) os = "LYNX";
#elif defined(Q_OS_MSDOS)
        if (os.isEmpty()) os = "MSDOS";
#elif defined(Q_OS_NETBSD)
        if (os.isEmpty()) os = "NETBSD";
#elif defined(Q_OS_OS2)
        if (os.isEmpty()) os = "OS2";
#elif defined(Q_OS_OPENBSD)
        if (os.isEmpty()) os = "OpenBSD";
#elif defined(Q_OS_OS2EMX)
        if (os.isEmpty()) os = "OS2EMX";
#elif defined(Q_OS_OSF)
        if (os.isEmpty()) os = "OSF";
#elif defined(Q_OS_QNX6)
        if (os.isEmpty()) os = "QNX6";
#elif defined(Q_OS_QNX)
        if (os.isEmpty()) os = "QNX";
#elif defined(Q_OS_RELIANT)
        if (os.isEmpty()) os = "RELIANT";
#elif defined(Q_OS_SCO)
        if (os.isEmpty()) os = "SCO";
#elif defined(Q_OS_SOLARIS)
        if (os.isEmpty()) os = "Solaris";
#elif defined(Q_OS_ULTRIX)
        if (os.isEmpty()) os = "ULTRIX";
#elif defined(Q_OS_UNIX)
        if (os.isEmpty()) os = "Unix";
#elif defined(Q_OS_UNIXWARE)
        if (os.isEmpty()) os = "UNIXWARE";
#endif
        if (os.isEmpty()) os = "Unknown";
        return os;
    }

    // Ruby の Shellwords.shellwords を書き直した
    QStringList shellwords(const QString &line_)
    {
        QString line = line_;
        QStringList words;
        QRegExp rx("^\"(([^\"\\\\]|\\\\.)*)\"");
        QRegExp rx2("^'([^']*)'");
        QRegExp rx3("^\\\\(.)?");
        QRegExp rx4("^([^\\s\\\\'\"]+)");
        while (!line.isEmpty()) {
            line.remove(QRegExp("^[\\s\\t\\r\\n\\f\\v]*"));
            QString field;
            forever {
                QString snippet;
                if (rx.indexIn(line) != -1) {
                    snippet = rx.cap(1).replace(QRegExp("\\\\(.)"), "\\1");
                    line.remove(rx);
                } else if (line.startsWith('"')) {
                    qWarning() << "Utils::shellwords: Unmatched double quote:" << line;
                    return QStringList();
                } else if (rx2.indexIn(line) != -1) {
                    snippet = rx2.cap(1);
                    line.remove(rx2);
                } else if (line.startsWith('\'')) {
                    qWarning() << "Utils::shellwords: Unmatched single quote:" << line;
                    return QStringList();
                } else if (rx3.indexIn(line) != -1) {
                    snippet = rx3.cap(1);
                    if (snippet.isEmpty())
                        snippet = "\\\\";
                    line.remove(rx3);
                } else if (rx4.indexIn(line) != -1) {
                    snippet = rx4.cap(1);
                    line.remove(rx4);
                } else {
                    break;
                }
                field.append(snippet);
            }
            words.append(field);
        }
        return words;
    }
}

