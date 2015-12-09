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

#include <QMutex>
#include "configuration.h"
#include "model/audio/playlistplayer.h"
#include "playlistentry.h"
#include "playlist.h"

Playlist* Playlist::instance = 0;

PlaylistEntry* Playlist::addEntry(QString filename, int pos)
{
    PlaylistEntry *newEntry = new PlaylistEntry(filename);
    if (pos == -1)
        entries.append(newEntry);
    else
        entries.insert(pos,newEntry);

    this->assignEntriesToPlayers();
    return newEntry;
}

void Playlist::removeItemFromPlaylist(PlaylistEntry *entry)
{
    emit playlistItemRemoved(entries.indexOf(entry));
    entries.removeOne(entry);
    playing.removeOne(entry);
    this->assignEntriesToPlayers();
}

void Playlist::removeItemFromPlaylist(int pos)
{
    PlaylistEntry *entry = entries.at(pos);
    entries.removeOne(entry);
    playing.removeOne(entry);
    this->assignEntriesToPlayers();
}

void Playlist::move(int pos1, int pos2)
{
    entries.move(pos1,pos2);
}

QList<PlaylistEntry*> Playlist::getEntries()
{
    return entries;
}

void Playlist::assignEntriesToPlayers()
{
    int lastItem=0;
    QMapIterator<int, PlaylistPlayer*> players(PlaylistPlayer::getPlayers());
    while (players.hasNext()) {
        players.next();
        PlaylistPlayer *player = players.value();
        if (!player->isPlaying())
        {
            bool loaded = false;
            for(int i=lastItem;i<entries.size();i++)
            {
                if (!playing.contains(entries.at(i)))
                {
                    player->loadEntry(entries.at(i));
                    emit playerAssigned(i,players.key());
                    lastItem = i+1;
                    loaded = true;
                    break;
                }
            }
            if (!loaded)
                player->loadEntry(NULL);
        }
    }
    for(int i=lastItem;i<entries.size();i++)
    {
        if (!playing.contains(entries.at(i)))
        emit playerAssigned(i,0);
    }
}

void Playlist::doAutoPlay(int player)
{
    if (!autoPlay)
        return;

    PlaylistPlayer::getObjectWithNumber(player)->fadeOut(Configuration::getInstance()->getPlaylistFade());
    QMapIterator<int, PlaylistPlayer*> players(PlaylistPlayer::getPlayers());
    while (players.hasNext()) {
        players.next();
        if (players.key() == player)
            continue;
        if (players.value()->isPlaying())
            continue;
        if (players.value()->getLoadedEntry() != 0)
        {
            players.value()->play();
            break;
        }
    }

}

void Playlist::fadeNext()
{
    int started;
    QMapIterator<int, PlaylistPlayer*> players2(PlaylistPlayer::getPlayers());
    while (players2.hasNext()) {
        players2.next();
        if (!players2.value()->isPlaying())
        {
            started = players2.key();
            if (players2.value()->getLoadedEntry() != NULL)
            players2.value()->play();
            break;
        }
    }
    int ms = Configuration::getInstance()->getPlaylistFade();
    QMapIterator<int, PlaylistPlayer*> players(PlaylistPlayer::getPlayers());
    while (players.hasNext()) {
        players.next();
        if (players.key() == started)
            continue;
        if (players.value()->isPlaying())
            players.value()->fadeOut(ms);
    }
}

void Playlist::addItemToPlaying(PlaylistEntry *entry)
{
    playing.append(entry);
}

void Playlist::setAutoPlay(bool set)
{
    this->autoPlay = set;
}

Playlist* Playlist::getInstance()
{
    static QMutex mutex;
    if (!instance)
    {
        mutex.lock();

        if (!instance)
            instance = new Playlist;

        instance->autoPlay = false;

        mutex.unlock();
    }
    return instance;
}

void Playlist::dropInstance()
{
    static QMutex mutex;
    mutex.lock();
    delete instance;
    instance = 0;
    mutex.unlock();
}
