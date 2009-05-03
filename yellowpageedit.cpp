/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "yellowpageedit.h"
#include "yellowpagedialog.h"
#include "yellowpage.h"
#include "yellowpagemanager.h"

class YellowPageListWidgetItem : public QTreeWidgetItem
{
public:
    YellowPageListWidgetItem(QTreeWidget *parent, YellowPage *yellowPage)
        : QTreeWidgetItem(parent), m_yellowPage(yellowPage)
    {
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
        update();
    }

    void update()
    {
        setText(YellowPageListWidget::Name, m_yellowPage->name());
        if (m_yellowPage->isEnabled())
            setCheckState(YellowPageListWidget::Name, Qt::Checked);
        else
            setCheckState(YellowPageListWidget::Name, Qt::Unchecked);
        setText(YellowPageListWidget::Url, m_yellowPage->url().toString());
        setText(YellowPageListWidget::NameSpace, m_yellowPage->nameSpaces().join("|"));
        setText(YellowPageListWidget::Type, YellowPage::typeString(m_yellowPage->type()));
    }

    YellowPage *yellowPage() const
    {
        return m_yellowPage;
    }

private:
    YellowPage *m_yellowPage;
};

//-------------------------------------------------------------------------------------------------

YellowPageListWidget::YellowPageListWidget(YellowPageManager *manager, QWidget *parent)
    : QTreeWidget(parent), m_manager(manager)
{
    setRootIsDecorated(false);
    QVector<QString> labels(LabelCount);
    labels[Name] = tr("名前");
    labels[Url] = tr("URL");
    labels[NameSpace] = tr("名前空間");
    labels[Type] = tr("種類");
    setHeaderLabels(labels.toList());
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem *, int)), SLOT(editCurrentItem()));
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
            this, SLOT(itemClicked(QTreeWidgetItem *, int)));
}

YellowPageListWidget::~YellowPageListWidget()
{
}

void YellowPageListWidget::setValue()
{
    foreach (YellowPage *yellowPage, m_manager->yellowPages())
        new YellowPageListWidgetItem(this, yellowPage);
}

void YellowPageListWidget::clear()
{
    QTreeWidget::clear();
    m_manager->clear();
    emit changed(true);
}

void YellowPageListWidget::upCurrentItem()
{
    QTreeWidgetItem *current = currentItem();
    int currentIndex = indexOfTopLevelItem(current);
    QTreeWidgetItem *above = itemAbove(current);
    int insertIndex = indexOfTopLevelItem(above);
    if (insertIndex != -1) {
        takeTopLevelItem(currentIndex);
        insertTopLevelItem(insertIndex, current);
        setCurrentItem(current);
        int i = m_manager->yellowPages().indexOf(
                static_cast<YellowPageListWidgetItem *>(current)->yellowPage());
        int j = m_manager->yellowPages().indexOf(
                static_cast<YellowPageListWidgetItem *>(above)->yellowPage());
        if (i != j) {
            m_manager->yellowPages().swap(i, j);
            emit changed(true);
        }
    }
}

void YellowPageListWidget::downCurrentItem()
{
    QTreeWidgetItem *current = currentItem();
    int currentIndex = indexOfTopLevelItem(current);
    QTreeWidgetItem *below = itemBelow(current);
    int insertIndex = indexOfTopLevelItem(below);
    if (insertIndex != -1) {
        takeTopLevelItem(currentIndex);
        insertTopLevelItem(insertIndex, current);
        setCurrentItem(current);
        int i = m_manager->yellowPages().indexOf(
                static_cast<YellowPageListWidgetItem *>(current)->yellowPage());
        int j = m_manager->yellowPages().indexOf(
                static_cast<YellowPageListWidgetItem  *>(below)->yellowPage());
        if (i != j) {
            m_manager->yellowPages().swap(i, j);
            emit changed(true);
        }
    }
}

void YellowPageListWidget::itemClicked(QTreeWidgetItem *i, int column)
{
    if (YellowPageListWidgetItem *item = static_cast<YellowPageListWidgetItem *>(i))
        if (column == Name) {
            bool stateChanged = item->yellowPage()->isEnabled() != (bool)item->checkState(column);
            item->yellowPage()->setEnabled((bool)item->checkState(column));
            if (stateChanged)
                emit changed(true);
        }
}

void YellowPageListWidget::addNewItem()
{
    YellowPage *yellowPage = new YellowPage();
    yellowPage->setName("");
    YellowPageDialog dialog(yellowPage, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->yellowPages().append(yellowPage);
        new YellowPageListWidgetItem(this, yellowPage);
        emit changed(true);
    } else {
        delete yellowPage;
    }
}

void YellowPageListWidget::editCurrentItem()
{
    editItem(currentItem());
}

void YellowPageListWidget::editItem(QTreeWidgetItem *i)
{
    YellowPageListWidgetItem *item = static_cast<YellowPageListWidgetItem *>(i);
    if (item) {
        YellowPage *yellowPage = item->yellowPage();
        YellowPageDialog dialog(yellowPage, this);
        if (dialog.exec() == QDialog::Accepted) {
            item->update();
            emit changed(true);
        }
    }
}

void YellowPageListWidget::removeCurrentItem()
{
    YellowPageListWidgetItem *item = static_cast<YellowPageListWidgetItem *>(currentItem());
    if (item) {
        int index = m_manager->yellowPages().indexOf(item->yellowPage());
        m_manager->yellowPages().removeAt(index);
        delete item->yellowPage();
        delete takeTopLevelItem(indexOfTopLevelItem(item));
        emit changed(true);
    }
}

void YellowPageListWidget::showEvent(QShowEvent *event)
{
    int width = header()->width();
    header()->resizeSection(Name, (int)(width * 0.23));
    header()->resizeSection(Url, (int)(width * 0.45));
    header()->resizeSection(NameSpace, (int)(width * 0.17));
    header()->resizeSection(Type, (int)(width * 0.15));
    QTreeWidget::showEvent(event);
}

void YellowPageListWidget::dropEvent(QDropEvent *event)
{
    emit changed(true);
    QTreeWidget::dropEvent(event);
}

void YellowPageListWidget::saveYellowPages()
{
    for (int i = 0; i < topLevelItemCount(); ++i) {
        YellowPageListWidgetItem *item = static_cast<YellowPageListWidgetItem *>(topLevelItem(i));
        int index = m_manager->yellowPages().indexOf(item->yellowPage());
        if (i != index)
            m_manager->yellowPages().swap(i, index);
    }
    m_manager->saveYellowPages();
}

//-------------------------------------------------------------------------------------------------

YellowPageEdit::YellowPageEdit(Settings *settings, QWidget *parent)
    : SettingWidget(parent), m_settings(settings)
{
    setupUi(this);
    m_manager = new YellowPageManager(settings);
    m_listWidget = new YellowPageListWidget(m_manager, this);
    m_listWidget->setDragDropMode(QAbstractItemView::InternalMove);
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

YellowPageEdit::~YellowPageEdit()
{
    delete m_manager;
}

void YellowPageEdit::setValue(bool reset)
{
    if (reset)
        m_listWidget->clear();
    else
        m_listWidget->setValue();
}

void YellowPageEdit::write()
{
    m_listWidget->saveYellowPages();
}

