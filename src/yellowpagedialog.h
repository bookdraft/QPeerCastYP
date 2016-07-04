/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef YELLOWPAGEDIALOG_H
#define YELLOWPAGEDIALOG_H

#include <QtGui>

#include "ui_yellowpagedialog.h"

class YellowPage;

class YellowPageDialog : public QDialog, private Ui::YellowPageDialog
{
    Q_OBJECT
public:
    YellowPageDialog(YellowPage *yellowPage, QWidget *parent = 0);
    virtual ~YellowPageDialog();

public slots:
    void accept();

private:
    YellowPage *m_yellowPage;
};

#endif // YELLOWPAGEDIALOG_H
