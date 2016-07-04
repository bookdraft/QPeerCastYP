/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef USERACTIONS_H
#define USERACTIONS_H

class Settings;
class CommandAction;

class UserActions : public QObject
{
    Q_OBJECT
public:
    UserActions(Settings *settings, QObject *parent = 0);
    virtual ~UserActions();

    QList<CommandAction *> &actions();
    void clear();

    void loadActions();
    void saveActions();

private:
    QList<CommandAction *> m_actions;
    Settings *m_settings;
};

#endif // USERACTIONS_H
