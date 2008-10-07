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
#include "process.h"
#include "sound.h"
#include "tooltip.h"
#include "viemacsbindings.h"

class ChannelListWidgetItem : public QTreeWidgetItem
{
public:
    ChannelListWidgetItem(QTreeWidget *parent, Channel *channel, int type = Type)
        : QTreeWidgetItem(parent, type), m_channel(channel)
    {
        setTextAlignment(ChannelListWidget::Listeners, Qt::AlignRight | Qt::AlignVCenter);
        setTextAlignment(ChannelListWidget::Relays, Qt::AlignRight | Qt::AlignVCenter);
        setTextAlignment(ChannelListWidget::Score, Qt::AlignRight | Qt::AlignVCenter);
        setTextAlignment(ChannelListWidget::Bitrate, Qt::AlignRight | Qt::AlignVCenter);
        setTextAlignment(ChannelListWidget::Uptime, Qt::AlignRight | Qt::AlignVCenter);
        updateText();
        updateLink();
    }

    Channel *channel() const { return m_channel; }

    void updateText()
    {
        setText(ChannelListWidget::Name, m_channel->name());
        setText(ChannelListWidget::Description, m_channel->longDescription());
        setText(ChannelListWidget::Score, QString::number(m_channel->score()));
        setText(ChannelListWidget::Listeners, m_channel->listenersString());
        setText(ChannelListWidget::Relays, m_channel->relaysString());
        setText(ChannelListWidget::Uptime, m_channel->uptimeString());
        setText(ChannelListWidget::Bitrate, m_channel->bitrateString());
        setText(ChannelListWidget::Type, m_channel->type());

        updateToolTip();

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
            QPixmap pixmap(":/images/favorite.png");
            QIcon statusIcon = icon(ChannelListWidget::Status);
            if (!statusIcon.isNull()) {
                QPainter painter(&pixmap);
                QSize statusIconSize(pixmap.size() * 0.5625);
                QPoint point(pixmap.width() - statusIconSize.width(),
                             pixmap.height() - statusIconSize.height());
                painter.drawPixmap(point, statusIcon.pixmap(statusIconSize));
            }
            setIcon(ChannelListWidget::Status, QIcon(pixmap));
            setToolTip(ChannelListWidget::Status, QObject::tr("お気に入り (スコア: %1)").arg(m_channel->score()));
        }
        if (!m_channel->isPlayable())
            setToolTip(ChannelListWidget::Status, QObject::tr("再生不可"));

    }

    void updateLink()
    {
        ChannelListWidget *tw = static_cast<ChannelListWidget *>(treeWidget());
        if (tw->minimumItemHeight())
            setSizeHint(0, QSize(1, tw->minimumItemHeight()));
        if (tw->linkEnabled()) {
            bool enable = false;
            switch (tw->linkType()) {
            case ChannelListWidget::ChannelLink:
                enable = m_channel->isPlayable();
                break;
            case ChannelListWidget::ContactLink:
                enable = m_channel->hasContactUrl();
                break;
            }
            setLink(ChannelListWidget::Name, enable);
        } else {
            setLink(ChannelListWidget::Name, false);
        }
    }

    void setLink(int column, bool enable = true)
    {
        ChannelListWidget *tw = static_cast<ChannelListWidget *>(treeWidget());
        if (!enable) {
            setFont(column, QFont());
            if (!m_channel->isPlayable())
                setForeground(column, QBrush(tw->style()->standardPalette()
                        .color(QPalette::Disabled, QPalette::WindowText)));
            else
                setForeground(column, QBrush());
            return;
        }
        QFont f(font(column));
        f.setUnderline(tw->linkUnderline());
        f.setBold(tw->linkBold());
        setFont(column, f);
        if (tw->linkColor().isValid())
            setForeground(column, QBrush(tw->linkColor()));
    }

    void updateToolTip()
    {
        for (int i = 0; i < treeWidget()->header()->count(); ++i) {
            if (treeWidget()->header()->sectionSize(i) == 0)
                continue;
            if (textIsElided(i))
                setToolTip(i, text(i));
            else
                setToolTip(i, "");
        }
    }

    bool textIsElided(int column)
    {
        QFontMetrics metrics(font(column));
        QString s = text(column);
        return metrics.width(s) > treeWidget()->columnWidth(column) - 7;
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
    : QTreeWidget(parent), m_active(false), m_linkHovering(false)
{
    setYellowPage(yellowPage);
    m_lastUpdatedTime = QDateTime::currentDateTime();
    QVector<QString> labels(Labels);
    labels[Status] = tr("S");
    labels[Name] = tr("配信者");
    labels[Description] = tr("詳細");
    labels[Listeners] = tr("視聴者数");
    labels[Relays] = tr("リレー数");
    labels[Score] = tr("スコア");
    labels[Uptime] = tr("配信時間");
    labels[Bitrate] = tr("ビットレート");
    labels[Type] = tr("種類");
    setHeaderLabels(labels.toList());
    header()->setStretchLastSection(true);
    header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header(), SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(headerContextMenuRequested(QPoint)));

    Settings *s = qApp->settings();
    s->beginGroup("ChannelListWidget");
        header()->restoreState(s->value("HeaderState").toByteArray());
        setIconSize(s->value("IconSize").toSize());
        setAlternatingRowColors(s->value("AlternatingRowColors").toBool());
        if (alternatingRowColors())
            setStyleSheet(QString("QTreeWidget { alternate-background-color: %1; }")
                    .arg(s->value("AlternateBackgroundColor").toString()));
        m_customToolTip = s->value("CustomToolTip").toBool();
        m_linkType = (LinkType)s->value("LinkType").toInt();
        m_linkEnabled = s->value("LinkEnabled").toBool();
        m_linkColor = s->value("LinkColor",
                style()->standardPalette().link().color()).value<QColor>();
        m_linkBold = s->value("LinkBold").toBool();
        m_linkUnderline = s->value("LinkUnderline").toBool();
        m_linkStatusTipFormats[ChannelLink] = s->value("ChannelLinkStatusTipFormat",
                tr("$CHANNEL(NAME) | ビットレート $CHANNEL(BITRATE)kbps | "
                "視聴者数 $CHANNEL(LISTENERS_STRING) | 配信時間 $CHANNEL(UPTIME_STRING) | "
                "種類 $CHANNEL(TYPE)")).toString();
        m_linkStatusTipFormats[ContactLink] = s->value("ContactLinkStatusTipFormat",
                tr("$CHANNEL(CONTACT_URL)")).toString();
        m_minimumItemHeight = s->value("MinimumItemHeight").toInt();
    s->endGroup();

    int lastSection = -1;
    for (int i = 0; i < header()->count(); ++i)
        if (header()->sectionSize(i))
            lastSection = i;
    if (lastSection != -1)
        header()->resizeSection(lastSection, fontMetrics().width(labels[lastSection])
                                    + style()->pixelMetric(QStyle::PM_HeaderGripMargin) * 2);

    setAllColumnsShowFocus(true);
    setSortingEnabled(true);
    setRootIsDecorated(false);
    setMouseTracking(linkEnabled());

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
        updateCursor();
        updateActions();
    } else {
        disconnect(m_yellowPage, SIGNAL(updateDone(YellowPage *, bool)),
                   this, SLOT(done(YellowPage *, bool)));
    }
}

void ChannelListWidget::updateYellowPage()
{
    if (!yellowPageCount())
        return;
    m_needClear = true;
    if (m_yellowPage->isUpdating())
        m_yellowPage->stopUpdate();
    m_yellowPage->update();
    updateCursor();
}

void ChannelListWidget::done(YellowPage *yp, bool error)
{
    if (error)
        qApp->mainWindow()->showErrorMessage(yp->errorString());
    if (m_needClear)
        clear();
    m_needClear = false;
    addItems(yp->channels());
    qApp->mainWindow()->updateStatusBar();
    m_lastUpdatedTime = QDateTime::currentDateTime();
    updateActions();
    if (!m_yellowPage->isUpdating()) {
        updateCursor();
        qApp->mainWindow()->notifyChannels();
    }
}

QDateTime ChannelListWidget::lastUpdatedTime()
{
    return m_lastUpdatedTime;
}

void ChannelListWidget::updateItems()
{
    m_lastUpdatedTime = QDateTime::currentDateTime();
    clear();
    foreach (YellowPage *yp, m_yellowPage->yellowPages())
        addItems(yp->channels());
}

void ChannelListWidget::addItems(const ChannelList &channels)
{
    setUpdatesEnabled(false);
    bool showStoppedChannels =
        qApp->settings()->value("General/ShowStoppedChannels").toBool();
    bool dontShowMinusScoreChannels =
        qApp->settings()->value("General/DontShowMinusScoreChannels").toBool();
    foreach (Channel *channel, channels) {
        if (!showStoppedChannels and channel->status() & Channel::Stopped)
            continue;
        if (dontShowMinusScoreChannels and channel->score() < 0)
            continue;
        ChannelListWidgetItem *item = new ChannelListWidgetItem(this, channel);
        bool disable = !channel->isPlayable() or channel->status() & Channel::Stopped;
        item->setDisabled(disable);
    }
    sortItems(sortColumn(), header()->sortIndicatorOrder());
    setUpdatesEnabled(true);
}

void ChannelListWidget::currentItemChanged(QTreeWidgetItem *current)
{
    Q_UNUSED(current);
    QString message = Process::expandVars(m_linkStatusTipFormats[ChannelLink], currentChannel());
    qApp->mainWindow()->updateStatusBar(message);
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
            actions->openContactUrlAction()->setToolTip(url);
            actions->openContactUrlAction()->setStatusTip(url);
            actions->openContactUrlWith2chBrowserAction()->setStatusTip(url);
        } else {
            QString text = actions->openContactUrlAction()->text();
            actions->openContactUrlAction()->setToolTip(text);
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
    qApp->mainWindow()->updateStatusBar();
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

bool ChannelListWidget::customToolTip() const
{
    return m_customToolTip;
}

void ChannelListWidget::setCustomToolTip(bool enable)
{
    m_customToolTip = enable;
}

void ChannelListWidget::updateLinks()
{
    for (int i = 0; i < topLevelItemCount(); ++i) {
        ChannelListWidgetItem *item =
            static_cast<ChannelListWidgetItem *>(topLevelItem(i));
        item->updateLink();
    }
}

ChannelListWidget::LinkType ChannelListWidget::linkType() const
{
    return m_linkType;
}

void ChannelListWidget::setLinkType(int type)
{
    if (m_linkType == type)
        return;
    m_linkType = (LinkType)type;
    updateLinks();
}

bool ChannelListWidget::linkEnabled() const
{
    return m_linkEnabled;
}

void ChannelListWidget::setLinkEnabled(bool enable)
{
    if (m_linkEnabled == enable)
        return;
    m_linkEnabled = enable;
    setMouseTracking(enable);
    updateLinks();
}

bool ChannelListWidget::linkUnderline() const
{
    return m_linkUnderline;
}

void ChannelListWidget::setLinkUnderline(bool enable)
{
    if (m_linkUnderline == enable)
        return;
    m_linkUnderline = enable;
    updateLinks();
}

bool ChannelListWidget::linkBold() const
{
    return m_linkBold;
}

void ChannelListWidget::setLinkBold(bool enable)
{
    m_linkBold = enable;
}

QColor ChannelListWidget::linkColor() const
{
    return m_linkColor;
}

void ChannelListWidget::setLinkColor(const QColor &c)
{
    m_linkColor = c;
}

QString ChannelListWidget::linkStatusTipFormat(LinkType type) const
{
    return m_linkStatusTipFormats[type];
}

void ChannelListWidget::setLinkStatusTipFormat(LinkType type, const QString &format)
{
    m_linkStatusTipFormats[type] = format;
}

int ChannelListWidget::minimumItemHeight() const
{
    return m_minimumItemHeight;
}

void ChannelListWidget::setMinimumItemHeight(int height)
{
    m_minimumItemHeight = height;
}

QRect ChannelListWidget::linkTextRect(QTreeWidgetItem *item, Column column) const
{
    QRect rect = visualRect(indexFromItem(item, column));
    rect.setX(rect.x() + style()->pixelMetric(QStyle::PM_HeaderGripMargin));
    int width = QFontMetrics(item->font(column)).width(item->text(column));
    if (width < rect.width())
        rect.setWidth(width);
    return rect;
}

void ChannelListWidget::updateCursor()
{
    if (m_yellowPage->isUpdating()) {
        qApp->mainWindow()->setCursor(Qt::BusyCursor);
        viewport()->setCursor(Qt::BusyCursor);
    } else {
        qApp->mainWindow()->setCursor(Qt::ArrowCursor);
        viewport()->setCursor(Qt::ArrowCursor);
    }
}

bool ChannelListWidget::event(QEvent *event)
{
    if (event->type() == QEvent::PolishRequest) {
        polish();
    } else if (event->type() == QEvent::ToolTip) {
        return true;
    }
    return QTreeWidget::event(event);
}

void ChannelListWidget::leaveEvent(QEvent *event)
{
    if (m_linkHovering) {
        qApp->mainWindow()->updateStatusBar();
        updateCursor();
        m_linkHovering = false;
    }
    QTreeWidget::leaveEvent(event);
}

bool ChannelListWidget::viewportEvent(QEvent *event)
{
    if (customToolTip() and event->type() == QEvent::ToolTip) {
        QHelpEvent *he = static_cast<QHelpEvent *>(event);
        QPoint pos = he->pos();
        ChannelListWidgetItem *item = static_cast<ChannelListWidgetItem *>(itemAt(pos));
        if (item) {
            QModelIndex index = indexAt(he->pos());
            int column = index.column();
            QRect rect = visualRect(index);
            QString text = item->toolTip(column);
            if (!text.isEmpty()) {
                QPoint pos(viewport()->mapToGlobal(rect.topLeft()));
                QFontMetrics fm(item->font(column));
                pos.setY(pos.y() + qRound(rect.height() / 2.0 - fm.height() / 2.0));
                int top = 0;
                ToolTip::instance()->getContentsMargins(0, &top, 0, 0);
                pos.setY(pos.y() - top);
                ToolTip::showText(pos, text, viewport());
            } else {
                ToolTip::hideText();
            }
        }
        return true;
    }
    return QTreeWidget::viewportEvent(event);
}

void ChannelListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (linkEnabled()) {
        QModelIndex index = indexAt(event->pos());
        if (index.isValid() and index.column() == Name) {
            ChannelListWidgetItem *item = static_cast<ChannelListWidgetItem *>(itemAt(event->pos()));
            QRect rect = linkTextRect(item, Name);
            bool hovering = rect.contains(event->pos());
            if (m_linkType == ChannelLink and !item->channel()->isPlayable())
                hovering = false;
            else if (m_linkType == ContactLink and !item->channel()->hasContactUrl())
                hovering = false;

            if (hovering and !m_linkHovering) {
                viewport()->setCursor(Qt::PointingHandCursor);
                m_linkHovering = true;
            } else if (!hovering and m_linkHovering) {
                qApp->mainWindow()->updateStatusBar();
                updateCursor();
                m_linkHovering = false;
            }

            QString format = m_linkStatusTipFormats[m_linkType];
            QString msg = Process::expandVars(format, item->channel());
            if (hovering and qApp->mainWindow()->statusBar()->currentMessage() != msg) {
                qApp->mainWindow()->updateStatusBar(msg);
            }
        } else if (m_linkHovering) {
            qApp->mainWindow()->updateStatusBar();
            updateCursor();
            m_linkHovering = false;
        }
    }
    QTreeWidget::mouseMoveEvent(event);
}

void ChannelListWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);

    QTreeWidgetItem *item = currentItem();
    if (item && !item->isSelected())
        item->setSelected(true); 

    if (event->button() == Qt::LeftButton) {
    } else if (event->button() == Qt::RightButton) {
        updateActions();
        QMenu *menu = qApp->actions()->yellowPageMenu();
        menu->exec(QCursor::pos());
        delete menu;
    } else if (event->button() == Qt::MidButton) {
        openContactUrl();
    }
}

void ChannelListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (linkEnabled() and event->button() == Qt::LeftButton) {
        ChannelListWidgetItem *item =
            static_cast<ChannelListWidgetItem *>(itemAt(event->pos()));
        if (!item)
            return;
        QRect rect = linkTextRect(item, Name);
        if (rect.contains(event->pos())) {
            switch (m_linkType) {
            case ChannelLink:
                if (item->channel()->isPlayable())
                    playChannel();
                break;
            case ContactLink:
                if (item->channel()->hasContactUrl())
                    openContactUrl();
                break;
            }
        }
    }
    QTreeWidget::mouseReleaseEvent(event);
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
    QTreeWidget::hideEvent(event);
}

void ChannelListWidget::showEvent(QShowEvent *event)
{
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

