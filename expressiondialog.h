/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef EXPRESSIONDIALOG_H
#define EXPRESSIONDIALOG_H

#include <QtGui>

#include "ui_expressiondialog.h"
#include "channelmatcher.h"

class ExpressionDialog : public QDialog, private Ui::ExpressionDialog
{
    Q_OBJECT
public:
    enum MatchType { RegExp, Contains, StartsWith, Exactly };
    ExpressionDialog(ChannelMatcher::Expression *exp, QWidget *parent = 0);
    virtual ~ExpressionDialog();

public slots:
    void accept();
    void on_matchTypeComboBox_currentIndexChanged(int index);

private:
    ChannelMatcher::Expression *m_expression;
};

#endif // EXPRESSIONDIALOG_H
