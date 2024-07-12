/* ***************************************************************************
 * This file is part of Event Music Machine.
 *
 * Event Music Machine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Event Music Machine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Event Music Machine. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************* */

#include <QToolBar>
#include "tfselectionbar.h"

TFSelectionBar::TFSelectionBar(QWidget *parent) :
    TFAbstractToolbar(parent)
{
    toolbar->setAllowedAreas(Qt::TopToolBarArea);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->setOrientation(Qt::Horizontal);
    toolbar->setFixedHeight(50);
    toolbar->setIconSize(QSize(42,42));

    setFixedHeight(50);

    setStyleSheet(" \
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
                    height:25px; \
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
    ");
}
