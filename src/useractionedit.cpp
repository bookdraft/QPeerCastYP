/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "useractionedit.h"
#include "settings.h"
#include "useractions.h"
#include "commandaction.h"
#include "commandactiondialog.h"

class UserActionListWidgetItem : public QTreeWidgetItem
{
public:
    UserActionListWidgetItem(QTreeWidget *parent, CommandAction *action)
        : QTreeWidgetItem(parent), m_action(action)
    {
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
        update();
    }

    void update()
    {
        setText(UserActionListWidget::Name, m_action->text());
        setIcon(UserActionListWidget::Name, m_action->icon());
        if (m_action->isEnabled())
            setCheckState(UserActionListWidget::Name, Qt::Checked);
        else
            setCheckState(UserActionListWidget::Name, Qt::Unchecked);
        QString baseName = QFileInfo(m_action->program()).baseName();
        setText(UserActionListWidget::Command, baseName + " " + m_action->arguments());
    }

    CommandAction *action() const
    {
        return m_action;
    }

private:
    CommandAction *m_action;
};

//---------------------------------------------------------------------------

UserActionListWidget::UserActionListWidget(UserActions *actions, QWidget *parent)
    : QTreeWidget(parent), m_actions(actions)
{
    QVector<QString> labels(LabelCount);
    labels[Name] = tr("名前");
    labels[Command] = tr("コマンド");
    setHeaderLabels(labels.toList());
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
            this, SLOT(editCurrentItem()));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
            this, SLOT(itemClicked(QTreeWidgetItem *, int)));
}

UserActionListWidget::~UserActionListWidget()
{
}

void UserActionListWidget::clear()
{
    emit changed(true);
    QTreeWidget::clear();
}

void UserActionListWidget::loadActions()
{
    clear();
    foreach (CommandAction *action, m_actions->actions())
        new UserActionListWidgetItem(this, action);
}

void UserActionListWidget::saveActions()
{
    m_actions->actions().clear();
    for (int i = 0; i < topLevelItemCount(); ++i) {
        UserActionListWidgetItem *item = static_cast<UserActionListWidgetItem *>(topLevelItem(i));
        m_actions->actions() += item->action();
    }
    m_actions->saveActions();
}

void UserActionListWidget::upCurrentItem()
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
        parent->takeChild(currentIndex);
        parent->insertChild(insertIndex, current);
        setCurrentItem(current);
        emit changed(true);
    }
}

void UserActionListWidget::downCurrentItem()
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
        parent->takeChild(currentIndex);
        parent->insertChild(insertIndex, current);
        setCurrentItem(current);
        emit changed(true);
    }
}

void UserActionListWidget::addNewItem()
{
    CommandAction *action = new CommandAction(tr("unnamed"), m_actions);
    CommandActionDialog dialog(action, this);
    if (dialog.exec() == QDialog::Accepted) {
        new UserActionListWidgetItem(this, action);
        emit changed(true);
    } else {
        delete action;
    }
}

void UserActionListWidget::editCurrentItem()
{
    if (!currentItem())
        return;
    editItem(currentItem());
}

void UserActionListWidget::editItem(QTreeWidgetItem *i)
{
    UserActionListWidgetItem *item = static_cast<UserActionListWidgetItem *>(i);
    if (item) {
        CommandAction *action = item->action();
        CommandActionDialog dialog(action, this);
        if (dialog.exec() == QDialog::Accepted) {
            item->update();
            emit changed(true);
        } else {
        }
    }
}

void UserActionListWidget::removeCurrentItem()
{
    if (!currentItem())
        return;
    delete takeTopLevelItem(indexOfTopLevelItem(currentItem()));
    emit changed(true);
}

void UserActionListWidget::showEvent(QShowEvent *event)
{
    int width = header()->width();
    header()->resizeSection(Name, (int)(width * 0.3));
    header()->resizeSection(Command, (int)(width * 0.7));
    QTreeWidget::showEvent(event);
}

void UserActionListWidget::dropEvent(QDropEvent *event)
{
    emit changed(true);
    QTreeWidget::dropEvent(event);
}

void UserActionListWidget::itemClicked(QTreeWidgetItem *i, int column)
{
    if (UserActionListWidgetItem *item = static_cast<UserActionListWidgetItem *>(i))
        if (column == Name) {
            bool stateChanged = item->action()->isEnabled() != (bool)item->checkState(column);
            item->action()->setEnabled((bool)item->checkState(column));
            if (stateChanged)
                emit changed(true);
        }
}

//---------------------------------------------------------------------------

UserActionEdit::UserActionEdit(Settings *settings, QWidget *parent)
    : SettingWidget(parent), m_settings(settings)
{
    setupUi(this);
    m_actions = new UserActions(m_settings, this);
    m_listWidget = new UserActionListWidget(m_actions, this);
    m_listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_listWidget->setAllColumnsShowFocus(true);
    m_listWidget->setRootIsDecorated(false);
    widget->layout()->addWidget(m_listWidget);
    connect(m_listWidget, SIGNAL(changed(bool)), this, SLOT(setDirty(bool)));
    connect(addButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(addNewItem()));
    connect(editButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(editCurrentItem()));
    connect(removeButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(removeCurrentItem()));
    connect(upButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(upCurrentItem()));
    connect(downButton, SIGNAL(clicked(bool)), m_listWidget, SLOT(downCurrentItem()));
}

UserActionEdit::~UserActionEdit()
{
}

void UserActionEdit::write()
{
    m_listWidget->saveActions();
}

void UserActionEdit::setValue(bool reset)
{
    if (reset)
        m_listWidget->clear();
    else
        m_listWidget->loadActions();
}

