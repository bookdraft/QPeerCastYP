/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef FAVORITEGROUPDIALOG_H
#define FAVORITEGROUPDIALOG_H

#include "ui_favoritegroupdialog.h"
#include "channelmatcher.h"

class FavoriteGroupDialog : public QDialog, private Ui::FavoriteGroupDialog
{
    Q_OBJECT
public:
    FavoriteGroupDialog(ChannelMatcher::Expression *exp = 0, QWidget *parent = 0);
    virtual ~FavoriteGroupDialog();

public slots:
    void accept();

private:
    ChannelMatcher::Expression *m_expression;
};

#endif // FAVORITEGROUPDIALOG_H
