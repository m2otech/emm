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

#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include "tfcontrolbar.h"

TFControlBar::TFControlBar(QWidget *parent) :
    QWidget(parent)
{
    QSpacerItem *spacer;

    addButton = new QPushButton(this);
    addButton->setFixedSize(30,30);
    addButton->setIconSize(QSize(20,20));
    addButton->setIcon(QIcon(":/icons/add.png"));

    editButton = new QPushButton(this);
    editButton->setFixedSize(30,30);
    editButton->setIconSize(QSize(20,20));
    editButton->setIcon(QIcon(":/icons/pencil.png"));

    removeButton = new QPushButton(this);
    removeButton->setFixedSize(30,30);
    removeButton->setIconSize(QSize(20,20));
    removeButton->setIcon(QIcon(":/icons/delete.png"));

    spacer = new QSpacerItem(0,0,QSizePolicy::Expanding);

    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(6,4,6,4);
    mainLayout->setSpacing(5);
    mainLayout->addWidget(addButton);
    mainLayout->addWidget(editButton);
    mainLayout->addWidget(removeButton);
    mainLayout->addSpacerItem(spacer);

    setLayout(mainLayout);
    setAttribute(Qt::WA_StyledBackground);
    setFixedHeight(36);

    connect(addButton, SIGNAL(clicked()), this, SLOT(progessAdd()));
    connect(editButton,SIGNAL(clicked()),this,SLOT(progressEdit()));
    connect(removeButton,SIGNAL(clicked()), this, SLOT(progessRemove()));

    setStyleSheet(" \
           QWidget#controlBar { \
                    background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(57, 57, 57, 255), stop:1 rgba(180, 180, 180, 255)); \
                    border-bottom:1px solid rgba(57, 57, 57, 255); \
            } \
            QPushButton { \
                color: #333; \
                border: 2px solid #555; \
                border-radius: 11px; \
                padding: 5px; \
                background: qradialgradient(cx: 0.3, cy: -0.4, \
                fx: 0.3, fy: -0.4, \
                radius: 1.35, stop: 0 #fff, stop: 1 #888); \
            } \
            QPushButton:hover { \
                background: qradialgradient(cx: 0.3, cy: -0.4, \
                fx: 0.3, fy: -0.4, \
                radius: 1.35, stop: 0 #fff, stop: 1 #bbb); \
            } \
            QPushButton:pressed { \
                background: qradialgradient(cx: 0.4, cy: -0.1, \
                fx: 0.4, fy: -0.1, \
                radius: 1.35, stop: 0 #fff, stop: 1 #ddd); \
            } \
    ");
}

void TFControlBar::progessAdd()
{
    emit addClicked();
}

void TFControlBar::progressEdit()
{
    emit editClicked();
}

void TFControlBar::progessRemove()
{
    emit removeClicked();
}
