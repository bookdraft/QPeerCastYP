/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef YELLOWPAGEEDIT_H
#define YELLOWPAGEEDIT_H

#include <QtGui>

#include "ui_yellowpageedit.h"
#include "settingwidget.h"

class YellowPageManager;
class Settings;

class YellowPageListWidget;

class YellowPageEdit : public SettingWidget, private Ui::YellowPageEdit
{
    Q_OBJECT
public:
    YellowPageEdit(Settings *settings, QWidget *parent = 0);
    virtual ~YellowPageEdit();

    void write();
    void setValue(bool reset = false);

private:
    Settings *m_settings;
    YellowPageManager *m_manager;
    YellowPageListWidget *m_listWidget;
};

class YellowPageListWidget : public QTreeWidget
{
    Q_OBJECT
public:
    enum Column { Name, Url, NameSpace, Type, LabelCount };
    YellowPageListWidget(YellowPageManager *manager, QWidget *parent);
    virtual ~YellowPageListWidget();

    void setValue();
    void saveYellowPages();

public slots:
    void clear();
    void upCurrentItem();
    void downCurrentItem();
    void addNewItem();
    void editCurrentItem();
    void editItem(QTreeWidgetItem *i);
    void removeCurrentItem();

signals:
    void changed(bool dirty);

protected:
    void dropEvent(QDropEvent * event);
    void showEvent(QShowEvent *event);

private slots:
    void itemClicked(QTreeWidgetItem *i, int column);

private:
    YellowPageManager *m_manager;
};

#endif // YELLOWPAGEEDIT_H
