/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef VIEMACSBINDINGS_H
#define VIEMACSBINDINGS_H

#include <QtGui>

// Vim と Emacs っぽいキーバインドを提供するイベントフィルタ。
class ViEmacsBindings : public QObject
{
    Q_OBJECT
public:
    ViEmacsBindings(QWidget *parent, bool vi = true, bool emacs = true);
    virtual ~ViEmacsBindings();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void triggered(int index);

private:
    // QSignalMapper *m_signalMapper;
    bool m_vi;
    bool m_emacs;
};

#endif // VIEMACSBINDINGS_H
