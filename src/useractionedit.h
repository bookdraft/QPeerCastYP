/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef USERACTIONEDIT_H
#define USERACTIONEDIT_H

#include <QtGui>
#include "settingwidget.h"
#include "ui_useractionedit.h"

class Settings;
class UserActions;
class UserActionListWidget;

class UserActionEdit : public SettingWidget, private Ui::UserActionEdit
{
    Q_OBJECT
public:
    UserActionEdit(Settings *settings, QWidget *parent = 0);
    virtual ~UserActionEdit();

    void write();
    void setValue(bool reset = false);

private:
    Settings *m_settings;
    UserActions *m_actions;
    UserActionListWidget *m_listWidget;
};

class UserActionListWidget : public QTreeWidget
{
    Q_OBJECT
public:
    enum Column { Name, Command, LabelCount };
    UserActionListWidget(UserActions *actions, QWidget *parent = 0);
    virtual ~UserActionListWidget();
    
    void loadActions();
    void saveActions();
    void clear();

public slots:
    void upCurrentItem();
    void downCurrentItem();

    void addNewItem();
    void editCurrentItem();
    void editItem(QTreeWidgetItem *i);
    void removeCurrentItem();

signals:
    void changed(bool dirty);

protected:
    void showEvent(QShowEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void itemClicked(QTreeWidgetItem *item, int column);

private:
    UserActions *m_actions;
};

#endif // USERACTIONEDIT_H
