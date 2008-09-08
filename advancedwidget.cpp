/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "advancedwidget.h"
#include "settings.h"

AdvancedWidget::AdvancedWidget(Settings *settings, QWidget *parent)
    : SettingWidget(parent), m_settings(settings)
{
    setupUi(this);
}

AdvancedWidget::~AdvancedWidget()
{
}

void AdvancedWidget::setValue(bool reset)
{
    Q_UNUSED(reset);
}

void AdvancedWidget::write()
{
}

