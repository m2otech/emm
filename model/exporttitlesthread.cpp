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
#include <QDateTime>

#include "exporttitlesthread.h"
#include "model/audio/cartslot.h"
#include "model/configuration.h"
#include "model/layerdata.h"
#include "view/cartslotwidget.h"

#include "view/mainwindow.h"

ExportTitlesThread::ExportTitlesThread(QList<int> layers, QString exportDir, QObject *parent) :
    QThread(parent)
{
    this->exportLayers = layers;
    this->exportDir = exportDir;
}

void ExportTitlesThread::run() {

    Configuration *config = Configuration::getInstance();

    int slotsPerLayer = config->getVerticalSlots()*config->getHorizontalSlots();

    // FILE TO EXPORT TO
    QString filename = exportDir + "/export" + QDateTime::currentDateTime().toString("_yyyyMMdd_hhmmss") + ".txt";

    QList<int> startingNumber;

    int number = 0;

    for (int i = 0; i < exportLayers.count(); i++)
        startingNumber.append((exportLayers.at(i) - 1) * slotsPerLayer + 1);

    emit updateMin(startingNumber.first());
    emit updateMax(startingNumber.last() + slotsPerLayer);

    // Remove file if existing (OVERWRITE)
    QFile file(filename);
    file.remove(filename);

    for (int i = 0; i < exportLayers.count(); i++)
    {
       number = startingNumber.at(i);

        // OPEN STREAM TO FILE
        if ( !file.open(QIODevice::Append) )
            return;

        QTextStream stream( &file );

        for (int j = 0; j < slotsPerLayer; j++)
        {
            CartSlot *slot = CartSlot::getObjectWithNumber(number);
            if (slot->getFileName() != "")
            {
               slot->loadFromSlot(number);

                // LAYER
                QString layer = config->getLayers().value(exportLayers.at(i) - 1)->getName();

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

                QString writeline = layer + '\t' + title + '\t' + starttime + '\t' + totaltime;

                // Write to file
                stream << writeline << endl;
           }
            number++;
            emit updateStatus(number);
        }
        file.close();
    }
}

void ExportTitlesThread::quit() {
    // TODO Find a way to interrupt the run thread and return cleanly (close file, ...)
}

