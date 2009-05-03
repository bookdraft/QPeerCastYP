/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QtGui>

class ToolTip : public QLabel
{
    Q_OBJECT
public:
    static ToolTip *instance();

    static void showText(const QPoint &pos, const QString &text, QWidget *widget = 0);
    static inline void hideText() { showText(QPoint(), QString()); }

protected:
    ToolTip(QWidget *parent = 0);
    virtual ~ToolTip();

    void show(const QPoint &pos, const QString &text, QWidget *widget = 0);

    QSize sizeHint(const QPoint &pos, QWidget *widget = 0);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void sendEventToWidget(QMouseEvent *event);
    static ToolTip *s_instance;
    QWidget *m_widget;
};

#endif // TOOLTIP_H
