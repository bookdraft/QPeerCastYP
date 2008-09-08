/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "viemacsbindings.h"

// static int const bindings[][4] = {
//     {Qt::ControlModifier | Qt::Key_F, Qt::Key_Right, Qt::NoModifier},
//     {Qt::ControlModifier | Qt::Key_B, Qt::Key_Left, Qt::NoModifier},
//     {Qt::ControlModifier | Qt::Key_N, Qt::Key_Down, Qt::NoModifier},
//     {Qt::ControlModifier | Qt::Key_P, Qt::Key_Up, Qt::NoModifier},
//     {Qt::ControlModifier | Qt::Key_H, Qt::Key_Backspace, Qt::NoModifier},
//     {0, 0}
// };

ViEmacsBindings::ViEmacsBindings(QWidget *parent, bool vi, bool emacs)
    : QObject(parent), m_vi(vi), m_emacs(emacs)
{
    // qDebug() << "--------------------------------------";
    // m_signalMapper = new QSignalMapper(this);
    // for (int i = 0; bindings[i][0]; ++i) {
    //     // qDebug() << (*pos)[0] << (*pos)[1] << (*pos)[2] << (*pos)[3];
    //     qDebug() << bindings[i][0];
    //     QShortcut *shortcut = new QShortcut(parent);
    //     shortcut->setKey(QKeySequence(bindings[i][0]));
    //     shortcut->setContext(Qt::WidgetShortcut);
    //     connect(shortcut, SIGNAL(activated()), m_signalMapper, SLOT(map()));
    //     connect(shortcut, SIGNAL(activatedAmbiguously()), m_signalMapper, SLOT(map()));
    //     m_signalMapper->setMapping(shortcut, i);
    //     qDebug() << shortcut;
    // }
    // connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(triggered(int)));
}

ViEmacsBindings::~ViEmacsBindings()
{
}

void ViEmacsBindings::triggered(int)
{
    // qDebug() << i;
    // QKeyEvent e(QEvent::KeyPress, bindings[i][1], (Qt::KeyboardModifier)bindings[i][2]);
    // QCoreApplication::sendEvent(parent(), &e);
}

bool ViEmacsBindings::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        int mod = ke->modifiers();
        int key = ke->key();
        int sendMod = 0;
        int sendKey = 0;

        if (m_emacs) {
            switch (mod | key) {
            case Qt::CTRL | Qt::Key_B:
                sendKey = Qt::Key_Left; break;
            case Qt::ALT | Qt::Key_B:
                sendMod = Qt::CTRL;
                sendKey = Qt::Key_Left; break;
            case Qt::CTRL | Qt::Key_F:
                sendKey = Qt::Key_Right; break;
            case Qt::ALT | Qt::Key_F:
                sendMod = Qt::CTRL;
                sendKey = Qt::Key_Right; break;
            case Qt::CTRL | Qt::Key_P:
                sendKey = Qt::Key_Up; break;
            case Qt::CTRL | Qt::Key_N:
                sendKey = Qt::Key_Down; break;
            case Qt::CTRL | Qt::Key_A:
                sendKey = Qt::Key_Home; break;
            case Qt::CTRL | Qt::Key_E:
                sendKey = Qt::Key_End; break;
            case Qt::CTRL | Qt::Key_D:
                sendKey = Qt::Key_Delete; break;
            case Qt::ALT | Qt::Key_D:
                sendMod = Qt::CTRL;
                sendKey = Qt::Key_Delete; break;
            case Qt::CTRL | Qt::Key_W:
                sendMod = Qt::CTRL;
                sendKey = Qt::Key_Backspace; break;
            }
        }

        if (m_vi) {
            switch (mod | key) {
            case Qt::Key_H:
                sendKey = Qt::Key_Left; break;
            case Qt::Key_J:
                sendKey = Qt::Key_Down; break;
            case Qt::Key_K:
                sendKey = Qt::Key_Up; break;
            case Qt::Key_L:
                sendKey = Qt::Key_Right; break;
            case Qt::Key_G:
                sendMod = Qt::CTRL;
                sendKey = Qt::Key_Home; break;
            case Qt::SHIFT | Qt::Key_G:
                sendMod = Qt::CTRL;
                sendKey = Qt::Key_End; break;
            }
        }

        switch (mod | key) {
        case Qt::CTRL | Qt::Key_H:
            sendKey = Qt::Key_Backspace; break;
        case Qt::CTRL | Qt::Key_BracketLeft:
            sendKey = Qt::Key_Escape; break;
        }

        if (sendKey) {
            QKeyEvent e(QEvent::KeyPress, sendKey, (Qt::KeyboardModifier)sendMod);
            QCoreApplication::sendEvent(parent(), &e);
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

