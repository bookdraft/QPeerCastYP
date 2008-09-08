/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef CHANNELMATCHER_H
#define CHANNELMATCHER_H

#include <QtCore>

class Channel;
class Settings;

class ChannelMatcher
{
public:
    enum TargetFlag {
        Name            = 0x0001,
        LongDescription = 0x0002,
        Genre           = 0x0004,
        Description     = 0x0008,
        Playing         = 0x0070, // == Artist | Album | Title
        Artist          = 0x0010,
        Album           = 0x0020,
        Title           = 0x0040,
        Message         = 0x0080,
        ContactUrl      = 0x0100,
        Type            = 0x0200,
    };
    Q_DECLARE_FLAGS(TargetFlags, TargetFlag)

    class Expression
    {
    public:
        Expression() {
            enabled = true;
            point = 3;
            targetFlags = Name | LongDescription;
            matchFlags = Qt::MatchRegExp;
        }
        bool enabled;
        QString pattern;
        int point;
        int targetFlags;
        int matchFlags;
    };

    ChannelMatcher(Settings *settings = 0);
    virtual ~ChannelMatcher();

    void clear();

    int score(Channel *channel) const;

    static QString targetString(Expression *exp);

    QList<Expression *> &expressions();
    void addExpression(const QString &pattern, Qt::MatchFlags matchFlags, TargetFlags flags, int point);
    void loadExpressions();
    void saveExpressions();

private:
    QList<Expression *> m_expressions;
    QSettings *m_settings;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelMatcher::TargetFlags)

#endif // CHANNELMATCHER_H
