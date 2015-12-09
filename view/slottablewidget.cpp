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

#include <QHeaderView>
#include "cartslotwidget.h"
#include "model/configuration.h"
#include "slottablewidget.h"

SlotTableWidget::SlotTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);
}

void SlotTableWidget::updateSlots(int layer)
{
    Configuration *conf = Configuration::getInstance();

    int rows = conf->getVerticalSlots();
    int columns = conf->getHorizontalSlots();

    setRowCount(rows);
    setColumnCount(columns);

    for (int i = 0; i < rows; i++) {
        verticalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    for (int i = 0; i < columns; i++) {
        horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    int number = 1 + layer * rows * columns;
    int displayNumber = number;

    if (conf->getLayerKeyboardSync()) {
        displayNumber = 1;
    }

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            CartSlotWidget *slotWidget = (CartSlotWidget*) cellWidget(row, column);
            if (!slotWidget) {
                CartSlotWidget *newWidget = new CartSlotWidget(number, this);
                setCellWidget(row, column, newWidget);
            } else {
                // m2: set disconnect=true, possible side effects?
                slotWidget->setNewNumber(number, true, displayNumber);
            }
            number++;
            displayNumber++;
        }
    }
}
