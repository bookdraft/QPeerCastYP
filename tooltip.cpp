/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "tooltip.h"

ToolTip *ToolTip::s_instance = 0;

ToolTip::ToolTip(QWidget *parent)
    : QLabel(parent, Qt::ToolTip)
{
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    setPalette(QToolTip::palette());
    setFont(QToolTip::font());
    setWordWrap(true);
    setContentsMargins(3, 1, 3, 1);
}

ToolTip::~ToolTip()
{
}

ToolTip *ToolTip::instance()
{
    if (!s_instance)
        s_instance = new ToolTip();
    return s_instance;
}

void ToolTip::showText(const QPoint &pos, const QString &text, QWidget *widget)
{
    if (pos.isNull() or text.isEmpty()) {
        instance()->hide();
        return;
    }
    instance()->show(pos, text, widget);
}

void ToolTip::show(const QPoint &pos, const QString &text, QWidget *widget)
{
    setText(text);
    move(pos);
    resize(sizeHint(pos, widget));
    QLabel::show();
}

QSize ToolTip::sizeHint(const QPoint &pos, QWidget *widget)
{
    QPoint p = widget ? widget->mapFromGlobal(pos) : pos;
    int w = fontMetrics().width(text());
    int h = fontMetrics().height();
    int l, t, r, b;
    getContentsMargins(&l, &t, &r, &b);
    QSize size(w + l + r, h + t + b);
    QRect screen = widget ? widget->rect() : QApplication::desktop()->screenGeometry();
    if (size.width() > screen.width() - p.x()) {
        QRect rect(0, 0, screen.width() - p.x(), h);
        rect = fontMetrics().boundingRect(rect, Qt::TextWordWrap, text());
        size.setWidth(rect.width() + l + r);
        size.setHeight(rect.height() + t + b);
    }
    return size;
}

void ToolTip::mousePressEvent(QMouseEvent *event)
{
    hide();
    QLabel::mousePressEvent(event);
}

void ToolTip::wheelEvent(QWheelEvent *event)
{
    hide();
    QLabel::wheelEvent(event);
}

void ToolTip::leaveEvent(QEvent *event)
{
    hide();
    QLabel::leaveEvent(event);
}

void ToolTip::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter painter;
    painter.setBrush(QBrush(Qt::black));
    painter.begin(this);
    painter.drawRect(0, 0, width() - 1, height() - 1);
    painter.end();
}

