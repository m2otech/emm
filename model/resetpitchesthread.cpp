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

#include "resetpitchesthread.h"
#include "model/audio/cartslot.h"
#include "model/configuration.h"
#include "view/cartslotwidget.h"

#include "view/mainwindow.h"

ResetPitchesThread::ResetPitchesThread(int selectedLayer, QObject *parent) :
    QThread(parent)
{
    this->selectedLayer = selectedLayer;
}

void ResetPitchesThread::run() {
    Configuration *config = Configuration::getInstance();
    // m2: added +1 since function is 1-based
    int number = config->getVerticalSlots()*config->getHorizontalSlots()*selectedLayer + 1;
    int minus = number;
    emit updateMax(config->getVerticalSlots()*config->getHorizontalSlots());
    for (int i=0;i<config->getVerticalSlots();i++)
    {
        for (int j=0;j<config->getHorizontalSlots();j++)
        {
            CartSlot *slot = CartSlot::getObjectWithNumber(number);
            MainWindow *win = MainWindow::getInstance();
            // m2: this happens inside empty() now
            // slot->clearColor();
            slot->loadFromSlot(number);
            if (slot->isPlaying()) {
                win->pitchDisplayUpdate(slot->getPitch());
                // set it explicitly to apply immediately
                slot->setPitch(slot->getPitch());
            }
            number++;
            emit updateStatus(number-minus);
        }
    }
}

