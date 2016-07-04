/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "channelmatcher.h"
#include "channel.h"
#include "application.h"
#include "settings.h"
#include "utils.h"

ChannelMatcher::ChannelMatcher(Settings *settings, QObject *parent)
    : QObject(parent), m_settings(settings)
{
    if (!settings)
        m_settings = qApp->settings();
    m_rootGroup = new Expression(this);
    m_rootGroup->isGroup = true;
    m_rootGroup->isRoot = true;
    loadExpressions();
}

ChannelMatcher::~ChannelMatcher()
{
}

typedef QPair<ChannelMatcher::TargetFlag, QString> TargetPair;

int ChannelMatcher::score(Channel *channel) const
{
    return score(channel, m_rootGroup);
}

int ChannelMatcher::score(Channel *channel, Expression *group) const
{
    int point = 0;
    QList<TargetPair> targetPairs;
    targetPairs << TargetPair(Name, channel->name())
                << TargetPair(LongDescription, channel->longDescription())
                << TargetPair(Genre, channel->genre())
                << TargetPair(Description, channel->description())
                << TargetPair(Artist, channel->artist())
                << TargetPair(Album, channel->album())
                << TargetPair(Title, channel->title())
                << TargetPair(Message, channel->message())
                << TargetPair(ContactUrl, channel->contactUrl().toString())
                << TargetPair(Type, channel->type());
    foreach (Expression *exp, group->expressions) {
        if (!exp->isEnabled or exp->pattern.isEmpty())
            continue;
        if (exp->isGroup) {
            point += score(channel, exp);
            continue;
        }
        bool matched = false;
        bool avoided = false;
        Qt::CaseSensitivity cs =
            exp->matchFlags & Qt::MatchCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
        QRegExp rx(exp->pattern, cs);
        foreach (TargetPair pair, targetPairs) {
            if (exp->targetFlags & pair.first) {
                if (exp->matchFlags & Qt::MatchRegExp) {
                    if (pair.second.contains(rx))
                        matched = true;
                } else if (exp->matchFlags & Qt::MatchContains) {
                    QStringList words = Utils::shellwords(exp->pattern);
                    bool targetMatched = false;
                    bool targetAvoided = false;
                    foreach (QString word, words) {
                        bool avoid = false;
                        if (word.startsWith('-')) {
                            avoid = true;
                            word.remove(0, 1);
                        }
                        bool b = pair.second.contains(word, cs);
                        if (b and avoid)
                            targetAvoided = true;
                        else if (b and !avoid)
                            targetMatched = true;
                    }
                    if (targetAvoided) {
                        avoided = true;
                        matched = false;
                    } else if (targetMatched) {
                        matched = true;
                    }
                } else if (exp->matchFlags & Qt::MatchStartsWith) {
                    if (pair.second.startsWith(exp->pattern, cs))
                        matched = true;
                } else if (exp->matchFlags == Qt::MatchExactly) {
                    if (pair.second == exp->pattern)
                        matched = true;
                }
            }
            if (avoided) {
                break;
            } else if (matched) {
                point += exp->point;
                break;
            }
        }
    }
    return point;
}


QString ChannelMatcher::targetString(Expression *exp)
{
    QStringList targets;
    if (exp->targetFlags & Name)
        targets += QObject::tr("配信者");
    if (exp->targetFlags & LongDescription)
        targets += QObject::tr("詳細");
    if (exp->targetFlags & Genre)
        targets += QObject::tr("ジャンル");
    if (exp->targetFlags & Description)
        targets += QObject::tr("短い詳細");
    if ((exp->targetFlags & Playing) == Playing) {
        targets += QObject::tr("現在の曲");
    } else {
        if (exp->targetFlags & Artist)
            targets += QObject::tr("アーティスト");
        if (exp->targetFlags & Album)
            targets += QObject::tr("アルバム");
        if (exp->targetFlags & Title)
            targets += QObject::tr("タイトル");
    }
    if (exp->targetFlags & Message)
        targets += QObject::tr("メッセージ");
    if (exp->targetFlags & ContactUrl)
        targets += QObject::tr("コンタクトURL");
    if (exp->targetFlags & Type)
        targets += QObject::tr("種類");
    return targets.join(", ");
}

QList<ChannelMatcher::Expression *> &ChannelMatcher::expressions()
{
    return m_rootGroup->expressions;
}

ChannelMatcher::Expression *ChannelMatcher::rootGroup() const
{
    return m_rootGroup;
}

void ChannelMatcher::addExpression(const QString &pattern, Qt::MatchFlags matchFlags, TargetFlags targetFlags, int point, Expression *group)
{
    if (!group)
        group = m_rootGroup;
    foreach (Expression *exp, group->expressions) {
        if (exp->pattern == pattern) {
            exp->targetFlags = targetFlags;
            exp->matchFlags = matchFlags;
            exp->point = point;
            return;
        }
    }
    Expression *exp = new Expression;
    exp->isEnabled = true;
    exp->pattern = pattern;
    exp->targetFlags = targetFlags;
    exp->matchFlags = matchFlags;
    exp->point = point;
    group->expressions += exp;
}

void ChannelMatcher::clear()
{
    delete m_rootGroup;
    m_rootGroup = new Expression(this);
    m_rootGroup->isGroup = true;
    m_rootGroup->isRoot = true;
}

bool ChannelMatcher::hasGroup() const
{
    foreach (Expression *exp, m_rootGroup->expressions)
        if (exp->isGroup)
            return true;
    return false;
}

void ChannelMatcher::loadExpressions()
{
    m_settings->beginGroup("Favorite");
    loadExpressions("Items", m_rootGroup);
    m_settings->endGroup();
}

void ChannelMatcher::loadExpressions(const QString &prefix, Expression *group)
{
    int size = m_settings->beginReadArray(prefix);
    for (int i = 0; i < size; ++i) {
        m_settings->setArrayIndex(i);
        Expression *exp = new Expression(group);
        if (!group->isRoot)
            exp->isChild = group->isGroup;
        exp->isGroup = m_settings->value("Group").toBool();
        exp->isEnabled = m_settings->value("Enabled").toBool();
        exp->pattern = m_settings->value("Pattern").toString();
        if (exp->isGroup) {
            loadExpressions(QString("Items"), exp);
        } else {
            exp->matchFlags = m_settings->value("MatchFlags").toInt();
            exp->targetFlags = m_settings->value("TargetFlags").toInt();
            exp->point = m_settings->value("Point").toInt();
        }
        group->expressions += exp;
    }
    m_settings->endArray();
}

void ChannelMatcher::saveExpressions()
{
    m_settings->remove("Favorite");
    m_settings->beginGroup("Favorite");
    saveExpressions("Items", m_rootGroup);
    m_settings->endGroup();
}

void ChannelMatcher::saveExpressions(const QString &prefix, Expression *group)
{
    m_settings->beginWriteArray(prefix);
    for (int i = 0; i < group->expressions.count(); ++i) {
        m_settings->setArrayIndex(i);
        Expression *exp = group->expressions[i];
        m_settings->setValue("Group", exp->isGroup);
        m_settings->setValue("Enabled", exp->isEnabled);
        m_settings->setValue("Pattern", exp->pattern);
        if (exp->isGroup) {
            saveExpressions("Items", exp);
        } else {
            m_settings->setValue("MatchFlags", exp->matchFlags);
            m_settings->setValue("TargetFlags", exp->targetFlags);
            m_settings->setValue("Point", exp->point);
        }
    }
    m_settings->endArray();
}

