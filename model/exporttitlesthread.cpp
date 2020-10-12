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

#include "exporttitlesthread.h"
#include "model/audio/cartslot.h"
#include "model/configuration.h"
#include "view/cartslotwidget.h"

#include "view/mainwindow.h"

ExportTitlesThread::ExportTitlesThread(int selectedLayer, QObject *parent) :
    QThread(parent)
{
    this->selectedLayer = selectedLayer;
    // selectedLayer >= 0 => current layer
    // selectedLayer = -1 => all layers
    // selectedLayer = -2 => slot store (TODO)
}

void ExportTitlesThread::run() {

    Configuration *config = Configuration::getInstance();

    int slotsPerLayer = config->getVerticalSlots()*config->getHorizontalSlots();
    int numberOfLayers = config->getLayers().count();

    // FILE TO EXPORT TO
    // TODO make file selectable
    QString filename;

    int number, minus;
    if (selectedLayer >= 0)
    {
        // m2: added +1 since function is 1-based
        number = slotsPerLayer * selectedLayer + 1;
        minus = number;
        numberOfLayers = 1;
        filename = "export_layer_" + QString("%1").arg(selectedLayer + 1) + ".txt";
        emit updateMax(slotsPerLayer);
    }
    else if (selectedLayer == -1)
    {
        number = 1;
        minus = number;
        filename = "export_layer_ALL.txt";;
        emit updateMax(slotsPerLayer * numberOfLayers);
    }

    // Remove file if existing (OVERWRITE)
    QFile file(filename);
    file.remove(filename);

    //for (int i = 0; i < config->getVerticalSlots(); i++)
    for (int i = 0; i < numberOfLayers; i++)
    {
        //for (int j = 0; j < config->getHorizontalSlots(); j++)
        for (int j = 0; j < slotsPerLayer; j++)
        {
            CartSlot *slot = CartSlot::getObjectWithNumber(number);
            MainWindow *win = MainWindow::getInstance();
            // m2: this happens inside empty() now
            // slot->clearColor();
            slot->loadFromSlot(number);

            // TITLE
            QString title = slot->getText1();

            // ARTIST
            // QString filename = slot->getFileName();
            // QFile file(filename);
            // TODO extract artist from MP3

            // TOTAL TIME
            double pos2 = slot->getLength();
            int mins2 = (int)pos2/60;
            int secs2 = (int)floor(pos2-mins2*60);
            int msecs2 = (int)floor((pos2-mins2*60-secs2)*10);
            QString totaltime = QString("%1:%2").arg(mins2,2, 10, QChar('0')).arg(secs2,2,10, QChar('0'));

            // START TIME
            pos2 = slot->getStartPos();
            mins2 = (int)pos2/60;
            secs2 = (int)floor(pos2-mins2*60);
            msecs2 = (int)floor((pos2-mins2*60-secs2)*1000);
            QString starttime = QString("%1:%2.%3").arg(mins2,2, 10, QChar('0')).arg(secs2,2,10, QChar('0')).arg(msecs2, 3, 10, QChar('0'));

            QString writeline = title + '\t' + starttime + '\t' + totaltime;

            if ( file.open(QIODevice::Append) )
            {
                QTextStream stream( &file );
                stream << writeline << endl;
                file.close();
            }

            number++;
            emit updateStatus(number-minus);
        }
    }
}

