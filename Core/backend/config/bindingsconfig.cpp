/***********************************************************************
Copyright (c) 2012 "Marco Gulino <marco.gulino@gmail.com>"

This file is part of Touché: https://github.com/rockman81/Touche

Touché is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details (included the COPYING file).

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include "bindingsconfig.h"
#include "domain/binding.h"
#include "domain/runcommandbinding.h"
#include "domain/tokeybinding.h"
#include "domain/donothingbinding.h"
#include <QMap>
#include <functional>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>

#define BINDING_DO_NOTHING "DoNothing"
#define BINDING_RUN_COMMAND "RunCommand"
#define BINDING_TO_KEY "TranslateToKey"

typedef std::function<Binding*(QObject*, const QString&)> BindingFactory;


class BindingsConfigPrivate {
public:
    BindingsConfigPrivate(BindingsConfig *parent) : settings(new QSettings("GuLinux", qAppName(), parent)) { }

    QMap<QString, BindingFactory> bindings;
    QSettings *settings;
    QString profile;
    DoNothingBinding doNothingBinding;

    Binding* moveToThreadAndReparent(QObject *newParent, QObject *object) {
        object->moveToThread(newParent->thread());
        object->setParent(newParent);
        return dynamic_cast<Binding*>(object);
    }

    void loadBindings() {
        settings->endGroup();
        settings->beginGroup(profile);
        const QString params = QString("%1/%2/%3");

        bindings[BINDING_RUN_COMMAND] = [this,params](QObject* p, const QString& eventName) {
            QString commandName = settings->value(params.arg(eventName, BINDING_RUN_COMMAND, "ApplicationName"), "true").toString();
            QStringList arguments = settings->value(params.arg(eventName, BINDING_RUN_COMMAND, "Arguments"), QStringList()).toStringList();
            return moveToThreadAndReparent(p, new RunCommandBinding(commandName, arguments));
        };
        bindings[BINDING_TO_KEY] =[this,params](QObject* p, const QString& eventName) {
            QString keySymbol = settings->value(params.arg(eventName, BINDING_TO_KEY, "keysymbol"), QString()).toString();
            QString isKeypress = settings->value(params.arg(eventName,BINDING_TO_KEY, "eventtype"), "keypress").toString();
            return moveToThreadAndReparent(p, new ToKeyBinding(keySymbol, isKeypress == "keypress", true));
        };
    }
};

BindingsConfig::BindingsConfig(QObject *parent) :
    QObject(parent), d_ptr(new BindingsConfigPrivate(this))
{
    Q_D(BindingsConfig);
    d->bindings[BINDING_DO_NOTHING] = [d](QObject* p, const QString& e) {
            Q_UNUSED(e);
            Q_UNUSED(p);
            return &d->doNothingBinding; };

}

BindingsConfig::~BindingsConfig()
{
    delete d_ptr;
}

Binding *BindingsConfig::bindingFor(const QString &eventName, QObject *parent)
{
    Q_D(BindingsConfig);
    QString bindingSetting = d->settings->value(eventName, BINDING_DO_NOTHING).toString();
    qDebug() << "Setting for event " << eventName << ": " << bindingSetting;
    BindingFactory bindingFactory = d->bindings.value(bindingSetting, d->bindings.value(BINDING_DO_NOTHING));
    return bindingFactory(parent, eventName);
}



void BindingsConfig::setCurrentProfile(const QString &profileName)
{
    Q_D(BindingsConfig);
    qDebug() << "Using profile " << profileName;
    d->profile = QString("bindings_%1").arg(profileName);
    d->loadBindings();
}


QStringList BindingsConfig::availableProfiles() const
{
    Q_D(const BindingsConfig);
    d->settings->endGroup();
    QStringList allProfiles = d->settings->childGroups();
    d->settings->beginGroup(d->profile);
    foreach(QString group, allProfiles)
        if(!group.startsWith("bindings_")) allProfiles.removeAll(group);
    return allProfiles.replaceInStrings("bindings_", "");
}


QString BindingsConfig::currentProfile() const
{
    Q_D(const BindingsConfig);
    return QString(d->profile).replace("bindings_", "");
}