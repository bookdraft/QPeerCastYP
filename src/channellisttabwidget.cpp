/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "channellisttabwidget.h"
#include "channellistwidget.h"
#include "yellowpage.h"
#include "application.h"
#include "actions.h"
#include "mainwindow.h"

class ChannelListTabBar : public QTabBar
{
    Q_OBJECT
public:
    ChannelListTabBar(QWidget *parent = 0) : QTabBar(parent) { }

protected:
    ChannelListTabWidget *tabWidget() const
    {
        return qobject_cast<ChannelListTabWidget *>(parentWidget());
    }

    void contextMenuEvent(QContextMenuEvent *event)
    {
        QMenu menu;
        menu.addAction(qApp->actions()->updateYellowPageAction());
        menu.addSeparator();
        for (int i = 0; i < count(); ++i) {
            ChannelListWidget *list = tabWidget()->widget(i);
            if (list and !list->yellowPage()->isManager()) {
                QAction *action = menu.addAction(QIcon(),
                        tr("%1 をウェブ・ブラウザで開く").arg(list->yellowPage()->name()));
                action->setProperty("url", list->yellowPage()->url());
            }
        }
        menu.addSeparator();
        QAction *hideTabBar = menu.addAction(tr("タブバーを隠す(&H)"));
        if (QAction *action = menu.exec(event->globalPos())) {
            if (!action->property("url").isNull())
                qApp->actions()->openUrl(action->property("url").toUrl());
            else if (action == hideTabBar)
                qApp->actions()->showTabBarAction()->trigger();
        }
    }

    void wheelEvent(QWheelEvent *event)
    {
        if (event->delta() > 0) {
            if (currentIndex() == 0)
                setCurrentIndex(count() - 1);
            else
                setCurrentIndex(currentIndex() - 1);
        } else {
            if (currentIndex() == count() - 1)
                setCurrentIndex(0);
            else
                setCurrentIndex(currentIndex() + 1);
        }
    }
};

ChannelListTabWidget::ChannelListTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setTabBar(new ChannelListTabBar(this));
    setDocumentMode(true);
    connect(this, SIGNAL(currentChanged(int)), SLOT(currentChanged(int)));
}

ChannelListTabWidget::~ChannelListTabWidget()
{
}

ChannelListWidget *ChannelListTabWidget::currentWidget()
{
    return qobject_cast<ChannelListWidget *>(QTabWidget::currentWidget());
}

ChannelListWidget *ChannelListTabWidget::widget(int index)
{
    return qobject_cast<ChannelListWidget *>(QTabWidget::widget(index));
}

void ChannelListTabWidget::currentChanged(int index)
{
    ChannelListWidget *current = widget(index);
    if (!current)
        return;
    for (int i = 0; i < count() and widget(i); ++i)
        if (i != index)
            widget(i)->setActive(false);
    current->setActive(true);
    if (current->yellowPage()->lastUpdatedTime() > current->lastUpdatedTime()) {
        current->updateItems();
    }
    qApp->mainWindow()->updateStatusBar();
}

void ChannelListTabWidget::findRequest(QString text, Qt::MatchFlags flags)
{
    if (currentWidget())
        currentWidget()->findItems(text, flags);
}

void ChannelListTabWidget::setActive(bool active)
{
    for (int i = 0; i < count() and widget(i); ++i)
        widget(i)->setActive(false);
    if (active)
        if (ChannelListWidget *current = currentWidget())
            current->setActive(true);
}

void ChannelListTabWidget::setTabBarVisible(bool shown)
{
    tabBar()->setVisible(shown);
}

bool ChannelListTabWidget::tabBarIsVisible() const
{
    return tabBar()->isVisible();
}

#include "channellisttabwidget.moc"
