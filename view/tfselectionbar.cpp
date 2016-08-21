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

#include <QToolBar>
#include "tfselectionbar.h"

extern bool DEMO, DEMO_M, DEMO_MT;

TFSelectionBar::TFSelectionBar(QWidget *parent) :
    TFAbstractToolbar(parent)
{
    int height = 25;

    if (DEMO_MT)
        height = 50;

    toolbar->setAllowedAreas(Qt::TopToolBarArea);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->setOrientation(Qt::Horizontal);
    toolbar->setFixedHeight(height);
    toolbar->setIconSize(QSize(21,21));

    setFixedHeight(height);

    const QString ssheet = " \
            QWidget { \
                    background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(57, 57, 57, 255), stop:1 rgba(180, 180, 180, 255)); \
                    border-bottom:1px solid rgba(57, 57, 57, 255); \
            } \
            QToolBar { \
                    border: 0; \
                    font-weight: bold; \
                    border-bottom:1px solid rgba(57, 57, 57, 255); \
                    spacing:0; \
            } \
            QToolBar:top { \
                    height:" + QString::number(height) + "px; \
                    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(57, 57, 57, 255), stop:1 rgba(130, 130, 130, 255)); \
            } \
            QToolButton { \
                    height:50px; \
                    border:0; \
                    padding-left:10px; \
                    padding-right:10px; \
                    border-left:1px qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(57, 57, 57, 255), stop:1 rgba(130, 130, 130, 255)); \
                    border-right:1px qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(57, 57, 57, 255), stop:1 rgba(130, 130, 130, 255)); \
                    border-bottom:1px solid rgba(57, 57, 57, 255); \
                    color:white; \
                    font-weight:bold; \
                    font-size:12px; \
            } \
            QToolButton:checked, QToolButton:selected { \
                color:#000000; \
                background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(196, 196, 196, 255), stop:1 rgba(252, 252, 252, 255)); \
                border-left:1px solid rgba(57, 57, 57, 255); \
                border-right:1px solid rgba(57, 57, 57, 255); \
            } \
            QToolButton:disabled { \
                color:#cccccc; \
            } \
    ";
    setStyleSheet(ssheet);
}
