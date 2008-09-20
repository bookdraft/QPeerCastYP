/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "favoritegroupdialog.h"

FavoriteGroupDialog::FavoriteGroupDialog(ChannelMatcher::Expression *exp, QWidget *parent)
    : QDialog(parent), m_expression(exp)
{
    setupUi(this);

    groupEdit->setText(exp->pattern);

    resize(minimumSizeHint().width() + 60, minimumSizeHint().height());
}

FavoriteGroupDialog::~FavoriteGroupDialog()
{
}

void FavoriteGroupDialog::accept()
{
    if (groupEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("エラー"), tr("グループ名を入力して下さい。"));
        return;
    } else {
        m_expression->pattern = groupEdit->text();
        QDialog::accept();
    }
}

