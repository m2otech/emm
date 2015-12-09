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

#include <QDir>
#include "bass.h"
#include "model/globaldata.h"
#include "pluginloader.h"

void PluginLoader::loadPlugins()
{
    QStringList files = QDir("plugins").entryList(QDir::Files);

    foreach(QString file, files)
    {
        HPLUGIN plugin = BASS_PluginLoad("plugins/"+file.toLatin1(),0);
        const BASS_PLUGININFO *info = BASS_PluginGetInfo(plugin);
        const BASS_PLUGINFORM *formats = info->formats;
        QString extensions = formats->exts;
        QStringList extensionList = extensions.split(";");
        foreach(QString format,extensionList)
        {
            GlobalData::addFilter(format);
        }
    }
}
