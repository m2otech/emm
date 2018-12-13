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

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>

class PlaylistEntry;

class Playlist : public QObject
{
    Q_OBJECT

public:
    static Playlist* getInstance();
    static void dropInstance();

    PlaylistEntry* addEntry(QString filename, int pos=-1);
    void move(int pos1, int pos2);
    QList<PlaylistEntry*> getEntries();

    double getRemainingLength();

private:
    Playlist() {}
    Playlist(const Playlist &);
    Playlist& operator=(const Playlist &);
    static Playlist* instance;

    QList<PlaylistEntry*> entries;
    QList<PlaylistEntry*> playing;
    bool autoPlay;

    double getTotalLength();


public slots:
    void assignEntriesToPlayers();
    void doAutoPlay(int player);
    void playPlayer(int player);
    void addItemToPlaying(PlaylistEntry* entry);
    void removeItemFromPlaylist(PlaylistEntry* entry);
    void removeItemFromPlaylist(int pos);
    void setAutoPlay(bool set);
    void fadeNext();

signals:
    void playlistItemRemoved(int pos);
    void playerAssigned(int pos, int number);

};

#endif // PLAYLIST_H
