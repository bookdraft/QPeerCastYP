/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "channellistwidget.h"
#include "actions.h"
#include "application.h"
#include "systemtrayicon.h"
#include "channel.h"
#include "channelmatcher.h"
#include "mainwindow.h"
#include "network.h"
#include "settings.h"
#include "settingsdialog.h"
#include "favoriteedit.h"
#include "yellowpage.h"
#include "yellowpagemanager.h"
#include "sound.h"
#include "viemacsbindings.h"

class ChannelListWidgetItem : public QTreeWidgetItem
{
public:
    ChannelListWidgetItem(QTreeWidget *parent, Channel *channel, int type = Type)
        : QTreeWidgetItem(parent, type), m_channel(channel)
    {
        setTextAlignment(ChannelListWidget::Listeners, Qt::AlignRight);
        setTextAlignment(ChannelListWidget::Relays, Qt::AlignRight);
        setTextAlignment(ChannelListWidget::Score, Qt::AlignRight);
        setTextAlignment(ChannelListWidget::Bitrate, Qt::AlignRight);
        setTextAlignment(ChannelListWidget::Uptime, Qt::AlignRight);
        updateText();
    }

    Channel *channel() const { return m_channel; }

    void updateText()
    {
        setText(ChannelListWidget::Name, m_channel->name());
        setText(ChannelListWidget::Description, m_channel->longDescription());
        setText(ChannelListWidget::Score, QString::number(m_channel->score()));
        setText(ChannelListWidget::Listeners,
                m_channel->listeners() < 0 ? "-" : QString::number(m_channel->listeners()));
        setText(ChannelListWidget::Relays,
                m_channel->relays() < 0 ? "-" : QString::number(m_channel->relays()));
        setText(ChannelListWidget::Uptime, m_channel->uptimeString());
        setText(ChannelListWidget::Bitrate, QString::number(m_channel->bitrate()) + "kb/s");
        setText(ChannelListWidget::Type, m_channel->type());

        updateToolTip();

        if (m_channel->listeners() < 0)
            setToolTip(ChannelListWidget::Listeners, QString::number(m_channel->listeners()));
        if (m_channel->relays() < 0)
            setToolTip(ChannelListWidget::Relays, QString::number(m_channel->relays()));

        Channel::Status status = m_channel->status();
        if (status & Channel::New) {
            setIcon(ChannelListWidget::Status, QIcon(":/images/new.png"));
            setToolTip(ChannelListWidget::Status, QObject::tr("新規"));
        } else if (status & Channel::Changed) {
            setIcon(ChannelListWidget::Status, QIcon(":/images/changed.png"));
            setToolTip(ChannelListWidget::Status, QObject::tr("詳細変更あり"));
        } else if (status & Channel::Normal) {
            setText(ChannelListWidget::Status, "");
        } else if (status & Channel::Stopped) {
            setIcon(ChannelListWidget::Status, QIcon(":/images/stopped.png"));
            setToolTip(ChannelListWidget::Status, QObject::tr("放送終了"));
        }
        if (status & Channel::Favorite) {
            if (status & Channel::New)
                setIcon(ChannelListWidget::Status, QIcon(":/images/favorite_new.png"));
            else if (status & Channel::Changed)
                setIcon(ChannelListWidget::Status, QIcon(":/images/favorite_changed.png"));
            else
                setIcon(ChannelListWidget::Status, QIcon(":/images/favorite.png"));
            setToolTip(ChannelListWidget::Status, QObject::tr("お気に入り (スコア: %1)").arg(m_channel->score()));
        }
        if (!m_channel->isPlayable())
            setToolTip(ChannelListWidget::Status, QObject::tr("再生不可"));

    }

    void updateToolTip()
    {
        for (int i = 0; i < treeWidget()->header()->count(); ++i) {
            if (treeWidget()->header()->sectionSize(i) == 0)
                continue;
            maybeSetToolTip(i);
        }
    }

    void maybeSetToolTip(int column)
    {
        QFontMetrics metrics(font(column));
        QString s = text(column);
        if (metrics.width(s) > treeWidget()->columnWidth(column) - 7)
            setToolTip(column, s);
        else
            setToolTip(column, "");
    }

    bool operator<(const QTreeWidgetItem &item) const
    {
        Channel *other = static_cast<const ChannelListWidgetItem *>(&item)->channel();
        switch (treeWidget()->sortColumn()) {
        case ChannelListWidget::Status:
            if (m_channel->status() + m_channel->score() == other->status() + other->score()) {
                if (m_channel->listeners() < -1 or other->listeners() < -1)
                    return m_channel->listeners() < other->listeners();
                else
                    return rand() % 2;
            } else {
                return m_channel->status() + m_channel->score() < other->status() + other->score();
            }
        case ChannelListWidget::Listeners:
            return m_channel->listeners() < other->listeners();
        case ChannelListWidget::Relays:
            return m_channel->relays() < other->relays();
        case ChannelListWidget::Score:
            return m_channel->score() < other->score();
        case ChannelListWidget::Bitrate:
            return m_channel->bitrate() < other->bitrate();
        case ChannelListWidget::Uptime:
            return m_channel->uptime() < other->uptime();
        default:
            return QTreeWidgetItem::operator<(item);
        }
    }

private:
    Channel *m_channel;
};

ChannelListWidget::ChannelListWidget(QWidget *parent, YellowPage *yellowPage)
    : QTreeWidget(parent), m_active(false)
{
    setYellowPage(yellowPage);
    m_lastUpdatedTime = QDateTime::currentDateTime();
    QVector<QString> headers(Headers);
    headers[Status] = tr("S");
    headers[Name] = tr("配信者");
    headers[Description] = tr("詳細");
    headers[Listeners] = tr("視聴者数");
    headers[Relays] = tr("リレー数");
    headers[Score] = tr("スコア");
    headers[Uptime] = tr("配信時間");
    headers[Bitrate] = tr("ビットレート");
    headers[Type] = tr("種類");
    setHeaderLabels(headers.toList());
    header()->setStretchLastSection(true);
    header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header(), SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(headerContextMenuRequested(QPoint)));
    QByteArray state = qApp->settings()->value("ChannelListWidget/HeaderState").toByteArray();
    header()->restoreState(state);

    int lastSection = -1;
    for (int i = 0; i < header()->count(); ++i)
        if (header()->sectionSize(i))
            lastSection = i;
    if (lastSection != -1)
        header()->resizeSection(lastSection, fontMetrics().width(headers[lastSection])
                                    + style()->pixelMetric(QStyle::PM_HeaderGripMargin) * 2);

    setAllColumnsShowFocus(true);
    setSortingEnabled(true);
    setRootIsDecorated(false);
    setIconSize(QSize(12, 12));

    setAlternatingRowColors(qApp->settings()->value("ChannelListWidget/AlternatingRowColors").toBool());
    if (alternatingRowColors())
        setStyleSheet(QString("QTreeWidget { alternate-background-color: %1; }")
                .arg(qApp->settings()->value("ChannelListWidget/AlternateBackgroundColor").toString()));

    installEventFilter(new ViEmacsBindings(this));
    updateActions();
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem *, int)), SLOT(playChannel()));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            SLOT(currentItemChanged(QTreeWidgetItem *)));
}

ChannelListWidget::~ChannelListWidget()
{
    if (m_yellowPage->isManager()) {
        QByteArray state = header()->saveState();
        qApp->settings()->setValue("ChannelListWidget/HeaderState", state);
    }
}

void ChannelListWidget::polish()
{
}

YellowPage *ChannelListWidget::yellowPage() const
{
    return m_yellowPage;
}

void ChannelListWidget::setYellowPage(YellowPage *yp)
{
    m_yellowPage = yp;
}

int ChannelListWidget::channelCount() const
{
    int count = 0;
    for (int i = 0; i < topLevelItemCount(); ++i)
        if (!topLevelItem(i)->isHidden())
            count++;
    return count;
}

int ChannelListWidget::listenerCount() const
{
    int count = 0;
    for (int i = 0; i < topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = topLevelItem(i);
        if (item->isHidden())
            continue;
        Channel *channel = static_cast<ChannelListWidgetItem *>(item)->channel();
        if (channel->listeners() > 0)
            count += channel->listeners();
    }
    return count;
}

int ChannelListWidget::yellowPageCount() const
{
    int count = 0;
    foreach (YellowPage *yp, m_yellowPage->yellowPages())
        if (yp->isEnabled())
            count++;
    return count;
}

bool ChannelListWidget::isActive() const
{
    return m_active;
}

void ChannelListWidget::setActive(bool active)
{
    if (m_active == active)
        return;
    m_active = active;
    if (active) {
        connect(m_yellowPage, SIGNAL(updateDone(YellowPage *, bool)),
                this, SLOT(done(YellowPage *, bool)));
        if (m_yellowPage->isUpdating())
            qApp->mainWindow()->setCursor(Qt::BusyCursor);
        else
            qApp->mainWindow()->setCursor(Qt::ArrowCursor);
        updateActions();
    } else {
        disconnect(m_yellowPage, SIGNAL(updateDone(YellowPage *, bool)),
                   this, SLOT(done(YellowPage *, bool)));
        qApp->mainWindow()->setCursor(Qt::ArrowCursor);
    }
}

void ChannelListWidget::updateYellowPage()
{
    if (!yellowPageCount())
        return;
    m_needClear = true;
    qApp->mainWindow()->setCursor(Qt::BusyCursor);
    if (m_yellowPage->isUpdating())
        m_yellowPage->stopUpdate();
    m_yellowPage->update();
}

void ChannelListWidget::done(YellowPage *yp, bool error)
{
    if (error)
        qApp->mainWindow()->showErrorMessage(yp->errorString());

    Settings *settings = qApp->settings();

    if (m_needClear)
        clear();
    m_needClear = false;
    addItems(yp->channels());
    if (settings->value("General/ShowStoppedChannels").toBool())
        addItems(yp->stoppedChannels());
    qApp->mainWindow()->updateStatusBar();
    m_lastUpdatedTime = QDateTime::currentDateTime();
    updateActions();

    if (!m_yellowPage->isUpdating()) {
        qApp->mainWindow()->setCursor(Qt::ArrowCursor);
        ChannelList noticeChannels;
        if (settings->value("Notification/NotifyFavorite").toBool()) {
            noticeChannels += m_yellowPage->channels(Channel::Favorite | Channel::New);
            if (settings->value("Notification/NotifyChangedFavorite").toBool())
                noticeChannels += m_yellowPage->channels(Channel::Favorite | Channel::Changed);
            int minimumScore = settings->value("Notification/MinimumScore").toInt();
            foreach (Channel *ch, noticeChannels)
                if (ch->score() < minimumScore)
                    noticeChannels.removeAll(ch);
        }
        if (settings->value("Notification/NotifyNew").toBool())
            noticeChannels += m_yellowPage->channels(Channel::New);
        if (settings->value("Notification/NotifyChanged").toBool())
            noticeChannels += m_yellowPage->channels(Channel::Changed);
        if (!noticeChannels .isEmpty()) {
            if (settings->value("Notification/ShowBalloonMessage").toBool())
                qApp->systemTrayIcon()->showFavoriteChannels(noticeChannels);
            if (settings->value("Notification/PlaySound").toBool())
                Sound::play(settings->value("Notification/SoundFile").toString());
        }
    }
}

QDateTime ChannelListWidget::lastUpdatedTime()
{
    return m_lastUpdatedTime;
}

void ChannelListWidget::updateItems()
{
    clear();
    m_lastUpdatedTime = QDateTime::currentDateTime();
    foreach (YellowPage *yp, m_yellowPage->yellowPages())
        addItems(yp->channels());
}

void ChannelListWidget::addItems(ChannelList &channels)
{
    setUpdatesEnabled(false);
    bool dontShowMinusScoreChannels
        = qApp->settings()->value("General/DontShowMinusScoreChannels").toBool();
    foreach (Channel *channel, channels) {
        if (dontShowMinusScoreChannels and channel->score() < 0)
            continue;
        ChannelListWidgetItem *item = new ChannelListWidgetItem(this, channel);
        bool disable = !channel->isPlayable() or channel->status() == Channel::Stopped;
        item->setDisabled(disable);
    }
    sortItems(sortColumn(), header()->sortIndicatorOrder());
    setUpdatesEnabled(true);
}

void ChannelListWidget::currentItemChanged(QTreeWidgetItem *current)
{
    Q_UNUSED(current);
    updateActions();
}

void ChannelListWidget::updateActions()
{
    Actions *actions = qApp->actions();
    bool hasYellowPage = (bool)yellowPageCount();
    actions->updateYellowPageAction()->setEnabled(hasYellowPage);
    actions->findChannelAction()->setEnabled(hasYellowPage);
    Channel *channel = currentChannel();
    bool enabled = (bool)channel;
    actions->playChannelAction()->setEnabled(enabled ? channel->isPlayable() : false);
    actions->addToFavoritesAction()->setEnabled(enabled);
    actions->copyChannelInfoAction()->setEnabled(enabled);
    actions->copyStreamUrlAction()->setEnabled(enabled);
    bool contactUrlAvailable = false;
    if (channel) {
        contactUrlAvailable = !channel->contactUrl().isEmpty();
        if (contactUrlAvailable) {
            QString url = channel->contactUrl().toString();
            actions->openContactUrlAction()->setStatusTip(url);
            actions->openContactUrlWith2chBrowserAction()->setStatusTip(url);
        } else {
            actions->openContactUrlAction()->setStatusTip("");
            actions->openContactUrlWith2chBrowserAction()->setStatusTip("");
        }
    }
    actions->openContactUrlAction()->setEnabled(contactUrlAvailable);
    actions->openContactUrlWith2chBrowserAction()->setEnabled(contactUrlAvailable);
    actions->copyContactUrlAction()->setEnabled(contactUrlAvailable);
}

void ChannelListWidget::filterItems(const QString &text, Qt::MatchFlags flags)
{
    Qt::CaseSensitivity cs =
        flags & Qt::MatchCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QRegExp::PatternSyntax ps =
        flags & Qt::MatchWildcard == Qt::MatchWildcard ? QRegExp::Wildcard : QRegExp::RegExp;
    QRegExp rx(text, cs, ps);

    for (int i = 0; i < topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = topLevelItem(i);
        if (!item) continue;
        bool matched = false;
        if (flags & Qt::MatchRegExp) {
            if (item->text(Name).contains(rx)
                    or item->text(Description).contains(rx)
                    or item->text(Type).contains(rx))
                matched = true;
        } else if (flags & Qt::MatchContains) {
            if (item->text(Name).contains(text, cs)
                    or item->text(Description).contains(text, cs)
                    or item->text(Type).contains(text, cs))
                matched = true;
        }
        item->setHidden(!matched);
    }
}

Channel *ChannelListWidget::currentChannel() const
{
    ChannelListWidgetItem *item = static_cast<ChannelListWidgetItem *>(currentItem());
    return item ? item->channel() : 0;
}

void ChannelListWidget::playChannel(Channel *channel)
{
    if (!channel)
        channel = currentChannel();

    qApp->actions()->playChannel(channel);
}

void ChannelListWidget::addToFavorites()
{
    if (Channel *channel = currentChannel()) {
        SettingsDialog dialog(qApp->settings(), this);
        dialog.setCurrentWidget(SettingsDialog::Favorite);
        dialog.favoriteEdit()->addExpression(channel->name(true), Qt::MatchStartsWith, ChannelMatcher::Name, 3);
        dialog.exec();
    }
}

void ChannelListWidget::openContactUrl()
{
    if (Channel *channel = currentChannel())
        qApp->actions()->openUrl(channel->contactUrl());
}

void ChannelListWidget::copyStreamUrl()
{
    if (Channel *channel = currentChannel())
        qApp->actions()->setClipboardText(channel->streamUrl().toString());
}

void ChannelListWidget::copyContactUrl()
{
    if (Channel *channel = currentChannel())
        qApp->actions()->setClipboardText(channel->contactUrl().toString());
}

void ChannelListWidget::copyChannelInfo()
{
    if (Channel *channel = currentChannel()) {
        QStringList info;
        info += channel->name();
        info += channel->longDescription();
        info += channel->streamUrl().toString();
        info += channel->contactUrl().toString();
        info.removeAll(QString());
        qApp->actions()->setClipboardText(info.join("\n"));
    }
}

bool ChannelListWidget::event(QEvent *event)
{
    if (event->type() == QEvent::PolishRequest)
        polish();
    return QTreeWidget::event(event);
}

void ChannelListWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);

    QTreeWidgetItem *item = currentItem();
    if (item && !item->isSelected())
        item->setSelected(true); 

    if (event->button() == Qt::LeftButton) {
        // 
    } else if (event->button() == Qt::RightButton) {
        updateActions();
        QMenu *menu = qApp->actions()->yellowPageMenu();
        menu->exec(QCursor::pos());
        delete menu;
    } else if (event->button() == Qt::MidButton) {
        openContactUrl();
    }
}

void ChannelListWidget::keyPressEvent(QKeyEvent *event)
{
    int mod = event->modifiers();
    int key = event->key();
    if (key == Qt::Key_Escape or (mod == Qt::CTRL and key == Qt::Key_G)) {
        filterItems("");
    } else {
        QTreeWidget::keyPressEvent(event);
    }
}

void ChannelListWidget::hideEvent(QHideEvent *event)
{
    // setActive(false);
    QTreeWidget::hideEvent(event);
}

void ChannelListWidget::showEvent(QShowEvent *event)
{
    // setActive(true);
    QTreeWidget::showEvent(event);
}

void ChannelListWidget::headerContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu menu(tr("カラムの表示"));
    for (int i = 0; i < header()->count(); ++i) {
        QAction *action = menu.addAction(headerItem()->text(i));
        action->setCheckable(true);
        action->setChecked(header()->sectionSize(i) != 0);
        action->setProperty("id", i);
    }
    if (QAction *action = menu.exec(QCursor::pos())) {
        int column = action->property("id").toInt();
        setColumnHidden(column, !action->isChecked());
    }
}

