/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "favoriteedit.h"
#include "application.h"
#include "channelmatcher.h"
#include "expressiondialog.h"

class ExpressionListWidgetItem : public QTreeWidgetItem
{
public:
    ExpressionListWidgetItem(QTreeWidget *parent, ChannelMatcher::Expression *exp)
        : QTreeWidgetItem(parent), m_expression(exp)
    {
        setTextAlignment(ExpressionListWidget::Point, Qt::AlignRight);
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
        update();
    }

    void update()
    {
        setText(ExpressionListWidget::Pattern, m_expression->pattern);
        if (m_expression->enabled)
            setCheckState(ExpressionListWidget::Pattern, Qt::Checked);
        else
            setCheckState(ExpressionListWidget::Pattern, Qt::Unchecked);
        setText(ExpressionListWidget::Target, ChannelMatcher::targetString(m_expression));
        setText(ExpressionListWidget::Point, QString::number(m_expression->point));
    }

    ChannelMatcher::Expression *expression() const
    {
        return m_expression;
    }

private:
    ChannelMatcher::Expression *m_expression;
};

//-------------------------------------------------------------------------------------------------

ExpressionListWidget::ExpressionListWidget(ChannelMatcher *matcher, QWidget *parent)
    : QTreeWidget(parent), m_matcher(matcher)
{
    setRootIsDecorated(false);
    QVector<QString> headers(Headers);
    headers[Pattern] = tr("検索文字/正規表現");
    headers[Target] = tr("検索対象");
    headers[Point] = tr("点数");
    setHeaderLabels(headers.toList());
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
            this, SLOT(editCurrentItem()));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
            this, SLOT(itemClicked(QTreeWidgetItem *, int)));
}

ExpressionListWidget::~ExpressionListWidget()
{
}

void ExpressionListWidget::setValue()
{
    foreach (ChannelMatcher::Expression *exp, m_matcher->expressions())
        new ExpressionListWidgetItem(this, exp);
}

void ExpressionListWidget::clear()
{
    QTreeWidget::clear();
    m_matcher->clear();
    emit changed(true);
}

void ExpressionListWidget::upCurrentItem()
{
    QTreeWidgetItem *current = currentItem();
    int currentIndex = indexOfTopLevelItem(current);
    QTreeWidgetItem *above = itemAbove(current);
    int insertIndex = indexOfTopLevelItem(above);
    if (insertIndex != -1) {
        takeTopLevelItem(currentIndex);
        insertTopLevelItem(insertIndex, current);
        setCurrentItem(current);
        int i = m_matcher->expressions().indexOf(
                static_cast<ExpressionListWidgetItem *>(current)->expression());
        int j = m_matcher->expressions().indexOf(
                static_cast<ExpressionListWidgetItem *>(above)->expression());
        if (i != j) {
            m_matcher->expressions().swap(i, j);
            emit changed(true);
        }
    }
}

void ExpressionListWidget::downCurrentItem()
{
    QTreeWidgetItem *current = currentItem();
    int currentIndex = indexOfTopLevelItem(current);
    QTreeWidgetItem *below = itemBelow(current);
    int insertIndex = indexOfTopLevelItem(below);
    if (insertIndex != -1) {
        takeTopLevelItem(currentIndex);
        insertTopLevelItem(insertIndex, current);
        setCurrentItem(current);
        int i = m_matcher->expressions().indexOf(
                static_cast<ExpressionListWidgetItem *>(current)->expression());
        int j = m_matcher->expressions().indexOf(
                static_cast<ExpressionListWidgetItem *>(below)->expression());
        if (i != j) {
            m_matcher->expressions().swap(i, j);
            emit changed(true);
        }
    }
}

void ExpressionListWidget::itemClicked(QTreeWidgetItem *i, int column)
{
    if (ExpressionListWidgetItem *item = static_cast<ExpressionListWidgetItem *>(i))
        if (column == Pattern) {
            bool stateChanged = item->expression()->enabled != (bool)item->checkState(column);
            item->expression()->enabled = (bool)item->checkState(column);
            if (stateChanged)
                emit changed(true);
        }
}

void ExpressionListWidget::addNewItem(ChannelMatcher::Expression *exp)
{
    if (!exp)
        exp = new ChannelMatcher::Expression;
    ExpressionDialog dialog(exp, this);
    if (dialog.exec() == QDialog::Accepted) {
        new ExpressionListWidgetItem(this, exp);
        m_matcher->expressions().append(exp);
        emit changed(true);
    } else {
        delete exp;
    }
}

void ExpressionListWidget::editCurrentItem()
{
    editItem(currentItem());
}

void ExpressionListWidget::editItem(QTreeWidgetItem *i)
{
    if (ExpressionListWidgetItem *item = static_cast<ExpressionListWidgetItem *>(i)) {
        ExpressionDialog dialog(item->expression(), this);
        if (dialog.exec() == QDialog::Accepted) {
            item->update();
            emit changed(true);
        }
    }
}

void ExpressionListWidget::removeCurrentItem()
{
    ExpressionListWidgetItem *item = static_cast<ExpressionListWidgetItem *>(currentItem());
    if (item) {
        int index = m_matcher->expressions().indexOf(item->expression());
        m_matcher->expressions().removeAt(index);
        delete item->expression();
        delete takeTopLevelItem(indexOfTopLevelItem(item));
        emit changed(true);
    }
}

void ExpressionListWidget::showEvent(QShowEvent *event)
{
    header()->resizeSection(Pattern, (int)(header()->width() * 0.6));
    header()->resizeSection(Target, (int)(header()->width() * 0.25));
    header()->resizeSection(Point, (int)(header()->width() * 0.15));
    QTreeWidget::showEvent(event);
}

void ExpressionListWidget::dropEvent(QDropEvent *event)
{
    emit changed(true);
    QTreeWidget::dropEvent(event);
}

void ExpressionListWidget::saveExpressions()
{
    for (int i = 0; i < topLevelItemCount(); ++i) {
        ExpressionListWidgetItem *item = static_cast<ExpressionListWidgetItem *>(topLevelItem(i));
        int index = m_matcher->expressions().indexOf(item->expression());
        if (i != index)
            m_matcher->expressions().swap(i, index);
    }
    m_matcher->saveExpressions();
}

//-------------------------------------------------------------------------------------------------

FavoriteEdit::FavoriteEdit(Settings *settings, QWidget *parent)
    : SettingWidget(parent), m_settings(settings)
{
    setupUi(this);
    m_matcher = new ChannelMatcher(settings);
    m_listWidget = new ExpressionListWidget(m_matcher, this);
    m_listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_listWidget->setAllColumnsShowFocus(true);
    m_listWidget->setFocus();
    widget->layout()->addWidget(m_listWidget);
    connect(m_listWidget, SIGNAL(changed(bool)), this, SLOT(setDirty(bool)));
    connect(addButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(addNewItem()));
    connect(editButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(editCurrentItem()));
    connect(removeButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(removeCurrentItem()));
    connect(upButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(upCurrentItem()));
    connect(downButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(downCurrentItem()));
}

FavoriteEdit::~FavoriteEdit()
{
    delete m_matcher;
}

void FavoriteEdit::setValue(bool reset)
{
    if (reset)
        m_listWidget->clear();
    else
        m_listWidget->setValue();
}

void FavoriteEdit::addExpression(const QString &pattern, Qt::MatchFlags matchFlags, ChannelMatcher::TargetFlags targetFlags, int point)
{
    ChannelMatcher::Expression *exp = new ChannelMatcher::Expression;
    exp->pattern = pattern;
    exp->matchFlags = matchFlags;
    exp->targetFlags = targetFlags;
    exp->point = point;
    m_listWidget->addNewItem(exp);
}

void FavoriteEdit::write()
{
    m_listWidget->saveExpressions();
}

