/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QtGui>

class SettingWidget : public QWidget
{
    Q_OBJECT
public:
    SettingWidget(QWidget *parent = 0);
    virtual ~SettingWidget();

    virtual void write() = 0;
    virtual void setValue(bool reset = false) = 0;

public slots:
    virtual bool isDirty() const;
    virtual void setDirty(bool dirty = true);

protected:
    bool m_dirty;
};

#endif // SETTINGWIDGET_H
