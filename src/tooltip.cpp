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
    : QLabel(parent, Qt::ToolTip), m_widget(0)
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
    m_widget = widget;
    setText(text);
    move(pos);
    resize(sizeHint(pos, widget));
    QLabel::show();
}

QSize ToolTip::sizeHint(const QPoint &pos, QWidget *widget)
{
    QPoint p = widget ? widget->mapFromGlobal(pos) : pos;
    int l, t, r, b;
    getContentsMargins(&l, &t, &r, &b);
    QRect screen = widget ? widget->rect() : QApplication::desktop()->screenGeometry();
    QRect rect(0, 0, screen.width() - p.x(), screen.height() - p.y());
    rect = fontMetrics().boundingRect(rect, Qt::TextWordWrap, text());
    return QSize(rect.width() + l + r, rect.height() + t + b);
}

void ToolTip::mousePressEvent(QMouseEvent *event)
{
    hide();
    sendEventToWidget(event);
}

void ToolTip::mouseReleaseEvent(QMouseEvent *event)
{
    sendEventToWidget(event);
}

void ToolTip::mouseDoubleClickEvent(QMouseEvent *event)
{
    sendEventToWidget(event);
}

void ToolTip::sendEventToWidget(QMouseEvent *event)
{
    if (m_widget) {
        QPoint gpos = event->globalPos();
        QMouseEvent e(event->type(), m_widget->mapFromGlobal(gpos), gpos,
                      event->button(), event->buttons(), event->modifiers());
        QCoreApplication::sendEvent(m_widget, &e);
    }
}

void ToolTip::wheelEvent(QWheelEvent *event)
{
    hide();
    if (m_widget) {
        QPoint gpos = event->globalPos();
        QWheelEvent e(m_widget->mapFromGlobal(gpos), gpos, event->delta(),
                      event->buttons(), event->modifiers(), event->orientation());
        QCoreApplication::sendEvent(m_widget, &e);
    }
}

void ToolTip::leaveEvent(QEvent *event)
{
    hide();
    QLabel::leaveEvent(event);
}

void ToolTip::paintEvent(QPaintEvent *event)
{
    QStylePainter painter(this);
    QStyleOptionFrame opt;
    opt.init(this);
    painter.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    painter.end();
    QLabel::paintEvent(event);
}

