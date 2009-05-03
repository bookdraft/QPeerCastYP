/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef FAVORITEEDIT_H
#define FAVORITEEDIT_H

#include <QtGui>

#include "ui_favoriteedit.h"
#include "channelmatcher.h"
#include "settingwidget.h"

class Settings;
class ExpressionListWidget;

class FavoriteEdit : public SettingWidget, private Ui::FavoriteEdit
{
    Q_OBJECT
public:
    FavoriteEdit(Settings *settings, QWidget *parent = 0);
    virtual ~FavoriteEdit();

    void addExpression(const QString &pattern, Qt::MatchFlags matchFlags, ChannelMatcher::TargetFlags targetFlags, int point);
    void write();
    void setValue(bool reset = false);

private:
    Settings *m_settings;
    ChannelMatcher *m_matcher;
    ExpressionListWidget *m_listWidget;
};

class ExpressionListWidget : public QTreeWidget
{
    Q_OBJECT
public:
    enum Column { Pattern, Target, Point, LabelCount };
    ExpressionListWidget(ChannelMatcher *matcher, QWidget *parent = 0);
    virtual ~ExpressionListWidget();

    void addItems();
    void addItems(QTreeWidgetItem *parent, ChannelMatcher::Expression *group);
    void saveExpressions();
    void saveExpressions(QTreeWidgetItem *item);

public slots:
    void clear();
    void upCurrentItem();
    void downCurrentItem();

    void addNewItem(ChannelMatcher::Expression *exp = 0);
    void addNewGroupItem();
    void editCurrentItem();
    void editItem(QTreeWidgetItem *i);
    void removeSelectedItem();

signals:
    void changed(bool dirty);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void itemClicked(QTreeWidgetItem *item, int column);

private:
    ChannelMatcher *m_matcher;
    QList<QTreeWidgetItem *> m_draggingItems;
};

#endif // FAVORITEEDIT_H
