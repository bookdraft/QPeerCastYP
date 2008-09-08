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

ChannelMatcher::ChannelMatcher(Settings *settings)
    : m_settings(settings)
{
    if (!settings)
        m_settings = qApp->settings();
    loadExpressions();
}

ChannelMatcher::~ChannelMatcher()
{
    clear();
}

typedef QPair<ChannelMatcher::TargetFlag, QString> TargetPair;

int ChannelMatcher::score(Channel *channel) const
{
    int score = 0;
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
    foreach (Expression *exp, m_expressions) {
        if (!exp->enabled or exp->pattern.isEmpty())
            continue;
        bool matched = false;
        Qt::CaseSensitivity cs =
            exp->matchFlags & Qt::MatchCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
        QRegExp rx(exp->pattern, cs);
        foreach (TargetPair pair, targetPairs) {
            if (exp->targetFlags & pair.first) {
                if (exp->matchFlags & Qt::MatchRegExp) {
                    if (pair.second.contains(rx))
                        matched = true;
                } else if (exp->matchFlags & Qt::MatchContains) {
                    if (pair.second.contains(exp->pattern, cs))
                        matched = true;
                } else if (exp->matchFlags & Qt::MatchStartsWith) {
                    if (pair.second.startsWith(exp->pattern, cs))
                        matched = true;
                } else if (exp->matchFlags == Qt::MatchExactly) {
                    if (pair.second == exp->pattern)
                        matched = true;
                }
            }
        }
        if (matched)
            score += exp->point;
    }
    return score;
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
    return m_expressions;
}

void ChannelMatcher::addExpression(const QString &pattern, Qt::MatchFlags matchFlags, TargetFlags targetFlags, int point)
{
    foreach (Expression *exp, m_expressions) {
        if (exp->pattern == pattern) {
            exp->targetFlags = targetFlags;
            exp->matchFlags = matchFlags;
            exp->point = point;
            return;
        }
    }
    Expression *exp = new Expression;
    exp->enabled = true;
    exp->pattern = pattern;
    exp->targetFlags = targetFlags;
    exp->matchFlags = matchFlags;
    exp->point = point;
    m_expressions += exp;
}

void ChannelMatcher::clear()
{
    while (!m_expressions.isEmpty())
        delete m_expressions.takeFirst();
}

void ChannelMatcher::loadExpressions()
{
    for (int i = 0; i < 100; ++i) {
        QString key = QString("Favorite/Item%1").arg(i, 2, 10, QChar('0'));
        if (!m_settings->contains(key))
            continue;
        Expression *exp = new Expression;
        exp->enabled = m_settings->value(key).toBool();
        exp->pattern = m_settings->value(key + "/Pattern").toString();
        exp->matchFlags = m_settings->value(key + "/MatchFlags").toInt();
        exp->targetFlags = m_settings->value(key + "/TargetFlags").toInt();
        exp->point = m_settings->value(key + "/Point").toInt();
        m_expressions += exp;
    }
}

void ChannelMatcher::saveExpressions()
{
    m_settings->remove("Favorite");
    for (int i = 0; i < m_expressions.count(); ++i) {
        Expression *exp = m_expressions[i];
        QString key = QString("Favorite/Item%1").arg(i, 2, 10, QChar('0'));
        m_settings->setValue(key, exp->enabled);
        m_settings->setValue(key + "/Pattern", exp->pattern);
        m_settings->setValue(key + "/MatchFlags", exp->matchFlags);
        m_settings->setValue(key + "/TargetFlags", exp->targetFlags);
        m_settings->setValue(key + "/Point", exp->point);
    }
}

