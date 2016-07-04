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

class ChannelMatcher : public QObject
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
    Q_DECLARE_FLAGS(TargetFlags, TargetFlag);

    class Expression : public QObject
    {
    public:
        Expression(QObject *parent = 0) : QObject(parent) {
            isRoot = false;
            isGroup = false;
            isChild = false;
            isEnabled = true;
            point = 3;
            targetFlags = Name | LongDescription;
            matchFlags = Qt::MatchContains;
        }
        ~Expression() { }
        bool isRoot;
        bool isGroup;
        bool isChild;
        bool isEnabled;
        QString pattern;
        int point;
        int targetFlags;
        int matchFlags;
        QList<Expression *> expressions;
    };

    ChannelMatcher(Settings *settings = 0, QObject *parent = 0);
    virtual ~ChannelMatcher();

    void clear();

    int score(Channel *channel) const;
    bool hasGroup() const;

    static QString targetString(Expression *exp);

    Expression *rootGroup() const;
    QList<Expression *> &expressions();
    void addExpression(const QString &pattern, Qt::MatchFlags matchFlags, TargetFlags flags, int point, Expression *group = 0);
    void loadExpressions();
    void saveExpressions();

private:
    int score(Channel *channel, Expression *exp) const;
    void loadExpressions(const QString &prefix, Expression *exp);
    void saveExpressions(const QString &prefix, Expression *exp);

    Expression *m_rootGroup;
    QSettings *m_settings;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelMatcher::TargetFlags)

#endif // CHANNELMATCHER_H
