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
#include "favoritegroupdialog.h"

class ExpressionListWidgetItem : public QTreeWidgetItem
{
public:
    ExpressionListWidgetItem(QTreeWidget *parent, ChannelMatcher::Expression *exp)
        : QTreeWidgetItem(parent), m_expression(exp)
    {
        setup();
    }

    ExpressionListWidgetItem(QTreeWidgetItem *parent, ChannelMatcher::Expression *exp)
        : QTreeWidgetItem(parent), m_expression(exp)
    {
        setup();
    }

    void setup()
    {
        setTextAlignment(ExpressionListWidget::Point, Qt::AlignRight);
        setExpanded(true);
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
        if (m_expression->isGroup)
            setFlags(flags() | Qt::ItemIsDropEnabled);
        update();
    }

    void update()
    {
        setText(ExpressionListWidget::Pattern, m_expression->pattern);
        if (m_expression->isEnabled)
            setCheckState(ExpressionListWidget::Pattern, Qt::Checked);
        else
            setCheckState(ExpressionListWidget::Pattern, Qt::Unchecked);
        if (!m_expression->isGroup) {
            setText(ExpressionListWidget::Target, ChannelMatcher::targetString(m_expression));
            setText(ExpressionListWidget::Point, QString::number(m_expression->point));
        }
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
    setRootIsDecorated(m_matcher->hasGroup());
    QVector<QString> labels(LabelCount);
    labels[Pattern] = tr("検索文字/正規表現");
    labels[Target] = tr("検索対象");
    labels[Point] = tr("点数");
    setHeaderLabels(labels.toList());
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
            this, SLOT(editCurrentItem()));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
            this, SLOT(itemClicked(QTreeWidgetItem *, int)));
}

ExpressionListWidget::~ExpressionListWidget()
{
}

void ExpressionListWidget::addItems()
{
    addItems(0, m_matcher->rootGroup());
}

void ExpressionListWidget::addItems(QTreeWidgetItem *parent, ChannelMatcher::Expression *group)
{
    foreach (ChannelMatcher::Expression *exp, group->expressions) {
        QTreeWidgetItem *current = 0;
        if (parent)
            current = new ExpressionListWidgetItem(parent, exp);
        else
            current = new ExpressionListWidgetItem(this, exp);
        if (exp->isGroup)
            addItems(current, exp);
    }
}

void ExpressionListWidget::clear()
{
    QTreeWidget::clear();
    m_matcher->clear();
    emit changed(true);
}

void ExpressionListWidget::upCurrentItem()
{
    if (!currentItem())
        return;
    QTreeWidgetItem *current = currentItem();
    QTreeWidgetItem *parent = current->parent();
    if (!parent)
        parent = invisibleRootItem();
    int currentIndex = parent->indexOfChild(current);
    int insertIndex = currentIndex - 1;
    if (insertIndex != -1) {
        bool expanded = current->isExpanded();
        parent->takeChild(currentIndex);
        parent->insertChild(insertIndex, current);
        setCurrentItem(current);
        current->setExpanded(expanded);
        emit changed(true);
    }
}

void ExpressionListWidget::downCurrentItem()
{
    if (!currentItem())
        return;
    QTreeWidgetItem *current = currentItem();
    QTreeWidgetItem *parent = current->parent();
    if (!parent)
        parent = invisibleRootItem();
    int currentIndex = parent->indexOfChild(current);
    int insertIndex = currentIndex + 1;
    if (insertIndex < parent->childCount()) {
        bool expanded = current->isExpanded();
        parent->takeChild(currentIndex);
        parent->insertChild(insertIndex, current);
        setCurrentItem(current);
        current->setExpanded(expanded);
        emit changed(true);
    }
}

void ExpressionListWidget::itemClicked(QTreeWidgetItem *i, int column)
{
    if (ExpressionListWidgetItem *item = static_cast<ExpressionListWidgetItem *>(i))
        if (column == Pattern) {
            bool stateChanged = item->expression()->isEnabled != (bool)item->checkState(column);
            item->expression()->isEnabled = (bool)item->checkState(column);
            if (stateChanged)
                emit changed(true);
        }
}

void ExpressionListWidget::addNewItem(ChannelMatcher::Expression *exp)
{
    if (!exp)
        exp = new ChannelMatcher::Expression(m_matcher->rootGroup());
    ExpressionDialog dialog(exp, this);
    if (dialog.exec() == QDialog::Accepted) {
        new ExpressionListWidgetItem(this, exp);
        m_matcher->expressions().append(exp);
        emit changed(true);
    } else {
        delete exp;
    }
}

void ExpressionListWidget::addNewGroupItem()
{
    ChannelMatcher::Expression *exp = new ChannelMatcher::Expression(m_matcher->rootGroup());
    exp->isGroup = true;
    FavoriteGroupDialog dialog(exp, this);
    if (dialog.exec() == QDialog::Accepted) {
        new ExpressionListWidgetItem(this, exp);
        m_matcher->expressions().append(exp);
        setRootIsDecorated(true);
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
        QDialog *dialog;
        if (item->expression()->isGroup)
            dialog = new FavoriteGroupDialog(item->expression(), this);
        else
            dialog = new ExpressionDialog(item->expression(), this);
        if (dialog->exec() == QDialog::Accepted) {
            item->update();
            emit changed(true);
        }
        delete dialog;
    }
}

void ExpressionListWidget::removeSelectedItem()
{
    foreach (QTreeWidgetItem *i, selectedItems()) {
        QTreeWidgetItem *parent = i->parent();
        if (!parent)
            parent = invisibleRootItem();
        parent->removeChild(i);
        emit changed(true);
    }
    if (currentItem())
        currentItem()->setSelected(true);
    bool hasGroup = false;
    for (int i = 0; i < topLevelItemCount(); ++i)
        if (static_cast<ExpressionListWidgetItem *>(
                    topLevelItem(i))->expression()->isGroup)
            hasGroup = true;
    setRootIsDecorated(hasGroup);
}

void ExpressionListWidget::showEvent(QShowEvent *event)
{
    header()->resizeSection(Pattern, (int)(header()->width() * 0.6));
    header()->resizeSection(Target, (int)(header()->width() * 0.25));
    header()->resizeSection(Point, (int)(header()->width() * 0.15));
    QTreeWidget::showEvent(event);
}

void ExpressionListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    m_draggingItems = selectedItems();
    bool group = false;
    bool exp = false;
    foreach (QTreeWidgetItem *item, m_draggingItems)
        if (static_cast<ExpressionListWidgetItem *>(item)->expression()->isGroup)
            group = true;
        else
            exp = true;
    if (group and exp)
        return;
    QTreeWidget::dragEnterEvent(event);
}

void ExpressionListWidget::dropEvent(QDropEvent *event)
{
    bool expanded = m_draggingItems.first()->isExpanded();
    QTreeWidgetItem *targetItem = itemAt(event->pos());
    if (!targetItem) {
        foreach (QTreeWidgetItem *item, m_draggingItems) {
            expanded = item->isExpanded();
            ExpressionListWidgetItem *i = static_cast<ExpressionListWidgetItem *>(item);
            QTreeWidgetItem *parent = i->parent();
            if (parent)
                parent->takeChild(parent->indexOfChild(i));
            else
                takeTopLevelItem(indexOfTopLevelItem(i));
            i->expression()->isChild = i->parent();
            insertTopLevelItem(topLevelItemCount(), i);
            i->setExpanded(expanded);
        }
        event->ignore();
        emit changed(true);
        return;
    }

    ChannelMatcher::Expression *target =
        static_cast<ExpressionListWidgetItem *>(targetItem)->expression();

    ChannelMatcher::Expression *drop =
        static_cast<ExpressionListWidgetItem *>(m_draggingItems.first())->expression();

    if ((target->isGroup and drop->isGroup and dropIndicatorPosition() == QAbstractItemView::OnItem)
            or (target->isChild and drop->isGroup)) {
        event->ignore();
        return;
    }

    QTreeWidget::dropEvent(event);

    bool hasParent = m_draggingItems.first()->parent();
    foreach (QTreeWidgetItem *item, m_draggingItems) {
        ExpressionListWidgetItem *i = static_cast<ExpressionListWidgetItem *>(item);
        i->expression()->isChild = hasParent;
        item->setExpanded(expanded);
    }
    emit changed(true);
}

void ExpressionListWidget::saveExpressions()
{
    saveExpressions(invisibleRootItem());
    m_matcher->saveExpressions();
}

void ExpressionListWidget::saveExpressions(QTreeWidgetItem *item)
{
    ChannelMatcher::Expression *group;
    if (item == invisibleRootItem())
        group = m_matcher->rootGroup();
    else
        group = static_cast<ExpressionListWidgetItem *>(item)->expression();
    group->expressions.clear();
    for (int i = 0; i < item->childCount(); ++i) {
        ExpressionListWidgetItem *item2 = static_cast<ExpressionListWidgetItem *>(item->child(i));
        group->expressions += item2->expression();
        if (item2->expression()->isGroup)
            saveExpressions(item2);
    }
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
    connect(addGroupButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(addNewGroupItem()));
    connect(editButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(editCurrentItem()));
    connect(removeButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(removeSelectedItem()));
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
        m_listWidget->addItems();
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

