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

#include <QFileInfo>
#include <QTimer>
#include "bass.h"
#include "playlistentry.h"
#include "tags.h"

PlaylistEntry::PlaylistEntry(QString filename, QObject *parent) :
        QObject(parent)
{
    BASS_SetDevice(1);
    HSTREAM stream = BASS_StreamCreateFile(false,filename.toLatin1(),0,0,0);
    artist = TAGS_Read(stream,"%ARTI");
    title = TAGS_Read(stream,"%TITL");
    if (artist == "" && title == "")
        artist = QFileInfo(filename).fileName().toLatin1();
    length = BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetLength(stream,BASS_POS_BYTE));
    BASS_StreamFree(stream);
    this->filename = filename;
}

QString PlaylistEntry::getArtist()
{
    return this->artist;
}

QString PlaylistEntry::getTitle()
{
    return this->title;
}

double PlaylistEntry::getLength()
{
    return this->length;
}

QString PlaylistEntry::getFilename()
{
    return this->filename;
}
