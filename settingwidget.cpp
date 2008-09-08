/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "settingwidget.h"

SettingWidget::SettingWidget(QWidget *parent)
    : QWidget(parent), m_dirty(false)
{
}

SettingWidget::~SettingWidget()
{
}

bool SettingWidget::isDirty() const
{
    return m_dirty;
}

void SettingWidget::setDirty(bool dirty)
{
    m_dirty = dirty;
}
