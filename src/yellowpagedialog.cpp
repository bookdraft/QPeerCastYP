/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "yellowpagedialog.h"
#include "yellowpage.h"

YellowPageDialog::YellowPageDialog(YellowPage *yellowPage, QWidget *parent)
    : QDialog(parent), m_yellowPage(yellowPage)
{
    setupUi(this);

    nameEdit->setText(m_yellowPage->name());
    urlEdit->setText(m_yellowPage->url().toString());
    nameSpaceEdit->setText(m_yellowPage->nameSpaces().join("|"));
    typeComboBox->insertItem(YellowPage::IndexTxt, YellowPage::typeString(YellowPage::IndexTxt));
    typeComboBox->setCurrentIndex(m_yellowPage->type());

    resize(minimumSizeHint());
}

YellowPageDialog::~YellowPageDialog()
{
}

void YellowPageDialog::accept()
{
    if (nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("エラー"), tr("名前を入力して下さい。"));
        return;
    }

    QUrl url(urlEdit->text());
    if (urlEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("エラー"), tr("URL を入力して下さい。"));
        return;
    } else if (!url.isValid()) {
        QMessageBox::warning(this, tr("エラー"), tr("無効な URL です。\n%1").arg(url.errorString()));
        return;
    }

    m_yellowPage->setName(nameEdit->text());
    m_yellowPage->setUrl(url);
    m_yellowPage->setNameSpaces(nameSpaceEdit->text().split("|", QString::SkipEmptyParts));
    m_yellowPage->setType((YellowPage::Type)typeComboBox->currentIndex());

    QDialog::accept();
}

