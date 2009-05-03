/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "application.h"
#ifdef Q_WS_X11
#include "proxystyle.h"
#endif
#ifdef Q_WS_WIN
#include "explorerstyle.h"
#endif

int main(int argc, char *argv[]) {
    Application app(argc, argv);
#ifdef Q_WS_X11
    app.setStyle(new ProxyStyle(app.style()->objectName()));
#endif
#ifdef Q_WS_WIN
    app.setStyle(new ExplorerStyle);
#endif
    int code = app.exec();
    return code;
}

