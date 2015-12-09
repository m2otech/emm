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
#include "globaldata.h"

QStringList GlobalData::additionalFilters = QStringList();

QString GlobalData::getColorCode(QString color)
{
    QString colorCode="#FFFFFF";
    if (color.toInt() > 0)
    {
        switch (color.toInt())
        {
        case 0: colorCode = "#FFFFFF"; break;
        case 1: colorCode = "#E1A6AD"; break;
        case 2: colorCode = "#FFA421"; break;
        case 3: colorCode = "#F3E03B"; break;
        case 4: colorCode = "#CBD0CC"; break;
        case 5: colorCode = "#48A43F"; break;
        case 6: colorCode = "#3481B8"; break;
        case 7: colorCode = "#EADEBD"; break;
        case 8: colorCode = "#FF0000"; break;
        }
    }
    else
    {
        if (color != "0")
            colorCode = color;
    }
    return colorCode;
}

QStringList GlobalData::getSupportedAudioFormats(bool detail)
{
    QStringList mainFilters;
    mainFilters << "*.mp3" << "*.mp2" << "*.mp1" << "*.ogg" << "*.wav" << "*.aiff";
    mainFilters.append(additionalFilters);

    QString files = tr("Dateien");
    QString supported = tr("Unterstüzte");

    QStringList filters;
    if (detail) {
        filters << supported+" "+files+" ("+mainFilters.join(" ")+")";
        foreach(QString format,mainFilters)
        {
            filters << format.split(".").last().toUpper()+" "+files+" ("+format+")";
        }
        return filters;
    }
    else
    {
        return mainFilters;
    }
}

void GlobalData::addFilter(QString filter)
{
    additionalFilters.append(filter);
}
