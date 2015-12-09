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

#include <QStringList>
#include "copycolorsthread.h"
#include "model/audio/cartslot.h"
#include "model/configuration.h"

CopyColorsThread::CopyColorsThread(int selectedLayer, QObject *parent) :
    QThread(parent)
{
    this->selectedLayer = selectedLayer;
}

void CopyColorsThread::run() {
    Configuration *config = Configuration::getInstance();
    QStringList fontColors;
    QStringList backgroundColors;
    fontColors.clear();
    backgroundColors.clear();
    // m2: added +1 since function is 1-based
    int number = config->getVerticalSlots()*config->getHorizontalSlots()*selectedLayer + 1;
    emit updateMax(config->getVerticalSlots()*config->getHorizontalSlots()*config->getLayer());
    for (int i=0;i<config->getVerticalSlots();i++)
    {
        for (int j=0;j<config->getHorizontalSlots();j++)
        {
            CartSlot *slot = CartSlot::getObjectWithNumber(number);
            fontColors.append(slot->getFontColor());
            backgroundColors.append(slot->getColor());
            number++;
        }
    }
    number = 0;
    for (int l=0;l<config->getLayer();l++)
    {
        int pos = 0;
        for (int i=0;i<config->getVerticalSlots();i++)
        {
            for (int j=0;j<config->getHorizontalSlots();j++)
            {
                CartSlot *slot = CartSlot::getObjectWithNumber(number);
                slot->setColorsAndSave(backgroundColors.at(pos),fontColors.at(pos));
                number++;
                pos++;
                emit updateStatus(number);
            }
        }
    }
}
