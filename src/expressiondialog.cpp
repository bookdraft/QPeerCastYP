/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "expressiondialog.h"
#include "channelmatcher.h"

ExpressionDialog::ExpressionDialog(ChannelMatcher::Expression *exp, QWidget *parent)
    : QDialog(parent), m_expression(exp)
{
    setupUi(this);

    patternEdit->setText(exp->pattern);

    int matchFlags = exp->matchFlags;
    if (matchFlags & Qt::MatchRegExp)
        matchTypeComboBox->setCurrentIndex(RegExp);
    else if (matchFlags & Qt::MatchContains)
        matchTypeComboBox->setCurrentIndex(Contains);
    else if (matchFlags & Qt::MatchStartsWith)
        matchTypeComboBox->setCurrentIndex(StartsWith);
    else if (matchFlags == Qt::MatchExactly)
        matchTypeComboBox->setCurrentIndex(Exactly);
    matchCaseSensitiveBox->setChecked((bool)(matchFlags & Qt::MatchCaseSensitive));

    int targetFlags = exp->targetFlags;
    targetNameBox->setChecked((bool)(targetFlags & ChannelMatcher::Name));
    targetLongDescriptionBox->setChecked((bool)(targetFlags & ChannelMatcher::LongDescription));
    targetGenreBox->setChecked((bool)(targetFlags & ChannelMatcher::Genre));
    targetPlayingBox->setChecked((bool)(targetFlags & ChannelMatcher::Playing));
    targetContactUrlBox->setChecked((bool)(targetFlags & ChannelMatcher::ContactUrl));
    targetTypeBox->setChecked((bool)(targetFlags & ChannelMatcher::Type));

    pointSpinBox->setValue(exp->point);

    resize(minimumSizeHint().width() + 20, minimumSizeHint().height());
}

ExpressionDialog::~ExpressionDialog()
{
}

void ExpressionDialog::on_matchTypeComboBox_currentIndexChanged(int index)
{
    if (index == Exactly) {
        matchCaseSensitiveBox->setChecked(false);
        matchCaseSensitiveBox->setEnabled(false);
    } else {
        matchCaseSensitiveBox->setEnabled(true);
    }
}

void ExpressionDialog::accept()
{
    if (patternEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("エラー"), tr("検索文字/正規表現を入力して下さい。"));
        return;
    }

    Qt::MatchFlags matchFlags = 0;
    switch (matchTypeComboBox->currentIndex()) {
    case RegExp: {
        QRegExp rx(patternEdit->text());
        if (!rx.isValid()) {
            QMessageBox::warning(this, tr("エラー"),
                    tr("正規表現が正しくありません。\n\nQRegExp: %1").arg(rx.errorString()));
            return;
        }
        matchFlags |= Qt::MatchRegExp;
        break;
    }
    case Contains:   matchFlags |= Qt::MatchContains;   break;
    case StartsWith: matchFlags |= Qt::MatchStartsWith; break;
    case Exactly:    matchFlags |= Qt::MatchExactly;    break;
    }

    ChannelMatcher::TargetFlags targetFlags = 0;
    if (targetNameBox->isChecked())
        targetFlags |= ChannelMatcher::Name;
    if (targetLongDescriptionBox->isChecked())
        targetFlags |= ChannelMatcher::LongDescription;
    if (targetGenreBox->isChecked())
        targetFlags |= ChannelMatcher::Genre;
    if (targetPlayingBox->isChecked())
        targetFlags |= ChannelMatcher::Playing;
    if (targetContactUrlBox->isChecked())
        targetFlags |= ChannelMatcher::ContactUrl;
    if (targetTypeBox->isChecked())
        targetFlags |= ChannelMatcher::Type;

    if (targetFlags == 0) {
        QMessageBox::warning(this, tr("エラー"), tr("検索対象を選択して下さい。"));
        return;
    }

    m_expression->pattern = patternEdit->text();
    if (matchCaseSensitiveBox->isChecked())
        matchFlags |= Qt::MatchCaseSensitive;
    m_expression->matchFlags = matchFlags;
    m_expression->targetFlags = targetFlags;
    m_expression->point = pointSpinBox->value();

    QDialog::accept();
}

