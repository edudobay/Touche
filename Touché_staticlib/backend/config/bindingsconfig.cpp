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

typedef std::function<Binding*(QObject*, const QString&)> BindingFactory;


class BindingsConfigPrivate {
public:
    BindingsConfigPrivate(BindingsConfig *parent) : settings(new QSettings("GuLinux", qAppName(), parent)) { }
    BindingFactory doNothingBindingFactory =  [this](QObject* p, const QString& e) {Q_UNUSED(e); return this->moveToThreadAndReparent(p, new DoNothingBinding()); };
    QMap<QString, BindingFactory> bindings;
    QSettings *settings;

    Binding* moveToThreadAndReparent(QObject *newParent, QObject *object) {
        object->moveToThread(newParent->thread());
        object->setParent(newParent);
        return dynamic_cast<Binding*>(object);
    }
};

BindingsConfig::BindingsConfig(QObject *parent) :
    QObject(parent), d_ptr(new BindingsConfigPrivate(this))
{
    Q_D(BindingsConfig);
    d->settings->beginGroup("bindings");
    const QString params = QString("%1/%2/%3");
    d->bindings["RunCommand"] = [d,params](QObject* p, const QString& eventName) {
        QString commandName = d->settings->value(params.arg(eventName, "RunCommand", "ApplicationName"), "true").toString();
        QStringList arguments = d->settings->value(params.arg(eventName, "RunCommand", "Arguments"), QStringList()).toStringList();
        return d->moveToThreadAndReparent(p, new RunCommandBinding(commandName, arguments));
    };
    d->bindings["TranslateToKey"] =[d,params](QObject* p, const QString& eventName) {
        QString keySymbol = d->settings->value(params.arg(eventName, "TranslateToKey", "keysymbol"), QString()).toString();
        QString isKeypress = d->settings->value(params.arg(eventName,"TranslateToKey", "eventtype"), "keypress").toString();
        return d->moveToThreadAndReparent(p, new ToKeyBinding(keySymbol, isKeypress == "keypress"));
    };

    qDebug() << "Configured bindings: ";
    foreach(const QString key, d->settings->childKeys()) {
        qDebug() << key << " [" << d->settings->value(key).toString() << "]";
    }

}

BindingsConfig::~BindingsConfig()
{
    delete d_ptr;
}

Binding *BindingsConfig::bindingFor(const QString &eventName, QObject *parent)
{
    Q_D(BindingsConfig);
    QString bindingSetting = d->settings->value(eventName, "DoNothing").toString();
    qDebug() << "Setting for event " << eventName << ": " << bindingSetting;
    BindingFactory bindingFactory = d->bindings.value(bindingSetting, d->doNothingBindingFactory);
    return bindingFactory(parent, eventName);
}

