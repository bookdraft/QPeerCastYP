/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "useractions.h"
#include "commandaction.h"
#include "settings.h"

UserActions::UserActions(Settings *settings, QObject *parent)
    : QObject(parent), m_settings(settings)
{
    loadActions();
}

UserActions::~UserActions()
{
}

void UserActions::clear()
{
    while (!m_actions.isEmpty())
        delete m_actions.takeFirst();
}

QList<CommandAction *> &UserActions::actions()
{
    return m_actions;
}

void UserActions::loadActions()
{
    clear();
    int size = m_settings->beginReadArray("UserAction/Actions");
    for (int i = 0; i < size; ++i) {
        m_settings->setArrayIndex(i);
        CommandAction *action = new CommandAction(m_settings->value("Text").toString(), this);
        bool enabled = m_settings->value("Enabled", true).toBool();
        action->setEnabled(enabled);
        action->setIcon(m_settings->value("Icon").toString());
        action->setCommand(m_settings->value("Program").toString(), m_settings->value("Args").toString());
        action->setShortcut(QKeySequence(m_settings->value("Shortcut").toString()));
        m_actions += action;
    }
    m_settings->endArray();
}

void UserActions::saveActions()
{
    m_settings->remove("UserAction/Actions");
    m_settings->beginWriteArray("UserAction/Actions");
    for (int i = 0; i < m_actions.size(); ++i) {
        m_settings->setArrayIndex(i);
        CommandAction *action = m_actions[i];
        m_settings->setValue("Enabled", action->isEnabled());
        m_settings->setValue("Text", action->text());
        m_settings->setValue("Icon", action->iconFileName());
        m_settings->setValue("Program", action->program());
        m_settings->setValue("Args", action->arguments());
        m_settings->setValue("Shortcut", action->shortcut().toString());
    }
    m_settings->endArray();
}

