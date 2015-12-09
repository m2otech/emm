/* ***************************************************************************
 * This file is part of EventMusicSoftware.
 *
 * EventMusicSoftware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EventMusicSoftware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EventMusicSoftware. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************* */

#include <QAction>
#include <QSignalMapper>
#include <QToolBar>
#include <QVBoxLayout>
#include "tfabstracttoolbar.h"

TFAbstractToolbar::TFAbstractToolbar(QWidget *parent) :
    QWidget(parent)
{
    toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);

    actionGroup = new QActionGroup(this);
    mapper = new QSignalMapper(this);

    connect(mapper, SIGNAL(mapped(int)), this, SLOT(buttonTriggered(int)));

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(toolbar);

    setLayout(mainLayout);

}

void TFAbstractToolbar::buttonTriggered(int index)
{
    emit currentTabChanged(index);
}

void TFAbstractToolbar::addButton(QAction *action)
{
    toolbar->addAction(action);
    actionGroup->addAction(action);
    mapper->setMapping(action, actions.size());
    connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
    actions.append(action);
}

void TFAbstractToolbar::selectFirstButton()
{
    actions.at(0)->setChecked(true);
}

int TFAbstractToolbar::getActionCount()
{
    return actions.size();
}

void TFAbstractToolbar::removeButtonAt(int pos)
{
    QAction *action = actions.takeAt(pos);
    actionGroup->removeAction(action);
    toolbar->removeAction(action);
    mapper->removeMappings(action);
}

int TFAbstractToolbar::getSelectedButton()
{
    for (int i=0;i<actions.count();i++) {
        if (actions.at(i)->isChecked())
            return i;
    }
    return 0;
}

void TFAbstractToolbar::selectButtonAt(int index)
{
    actions.at(index)->setChecked(true);
    emit currentTabChanged(index);
}

QAction* TFAbstractToolbar::getButtonAt(int index)
{
    return actions.at(index);
}

