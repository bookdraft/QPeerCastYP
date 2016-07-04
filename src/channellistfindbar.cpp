/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "channellistfindbar.h"
#include "application.h"
#include "mainwindow.h"
#include "channellistwidget.h"
#include "channellistfindbar.h"
#include "viemacsbindings.h"

bool ChannelListFindBarEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        int key = ke->key();
        if (key == Qt::Key_Escape)
            findBar->closeButton->click();
        if (key == Qt::Key_Up or key == Qt::Key_Down)
            qApp->mainWindow()->currentChannelListWidget()->setFocus();
        return true;
    }
    return QObject::eventFilter(watched, event);
}

ChannelListFindBar::ChannelListFindBar(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    closeButton->setIcon(QIcon(":/images/close.png").pixmap(16, 16));
    lineEdit->installEventFilter(new ViEmacsBindings(lineEdit, false));
    installEventFilter(new ChannelListFindBarEventFilter(this));
}

ChannelListFindBar::~ChannelListFindBar()
{
}

void ChannelListFindBar::show()
{
    lineEdit->setFocus();
    lineEdit->selectAll();
    QWidget::show();
}

void ChannelListFindBar::on_closeButton_clicked()
{
    lineEdit->setText("");
}

void ChannelListFindBar::on_caseSensitivityCheckBox_stateChanged()
{
    emitFindRequest();
}

void ChannelListFindBar::on_regExpCheckBox_stateChanged()
{
    emitFindRequest();
}

void ChannelListFindBar::on_lineEdit_returnPressed()
{
    emitFindRequest();
}

void ChannelListFindBar::on_lineEdit_textChanged()
{
    emitFindRequest();
}

void ChannelListFindBar::emitFindRequest()
{
    Qt::MatchFlags flags;

    if (caseSensitivityCheckBox->isChecked())
        flags |= Qt::MatchCaseSensitive;

    if (regExpCheckBox->isChecked())
        flags |= Qt::MatchRegExp;
    else
        flags |= Qt::MatchContains;

    emit findRequest(lineEdit->text(), flags);
}

