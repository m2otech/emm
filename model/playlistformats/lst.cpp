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

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include "lst.h"

QStringList LST::loadEntriesFromFile(QString fileName) {
    QStringList output;
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QTextStream out(&file);
    QString zeile;
    while(!out.atEnd())
    {
        zeile = out.readLine();
        zeile = zeile.split("|").first();
        output.append(zeile);
    }
    file.close();
    return output;
}
