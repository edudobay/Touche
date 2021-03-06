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

#ifndef RUNCOMMANDCONFIG_H
#define RUNCOMMANDCONFIG_H

#include "bindingconfigurationwidget.h"

class Ui_RunCommandConfig;
class KEditListBox;
class RunCommandConfigFactory : public BindingConfigurationWidgetFactory {
public:
    virtual BindingConfigurationWidget *build(QSettings *settings, const QString &bindingType,
                CfgKeyEvent *event, SiblingsList *, QWidget *parent);
};

class EditStringListWrapper : public QObject {
    Q_OBJECT
public:
    explicit EditStringListWrapper(const QString &objectName, KEditListBox *parent);
    void insertStringList(const QStringList &stringList);
signals:
    void stringListChanged(const QStringList &stringList);
private slots:
    void stringListChanged();
private:
    KEditListBox *parent;
};

class RunCommandConfig : public BindingConfigurationWidget
{
    Q_OBJECT
public:
    explicit RunCommandConfig(QSettings *settings, const QString &bindingType, CfgKeyEvent *event, QWidget *parent=0);
    virtual ~RunCommandConfig();
private:
    Ui_RunCommandConfig *ui;
    
};

#endif // RUNCOMMANDCONFIG_H
