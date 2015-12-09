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

#ifndef PLAYLISTPLAYER_H
#define PLAYLISTPLAYER_H

#include "abstractaudioobject.h"

class PlaylistEntry;

class PlaylistPlayer : public AbstractAudioObject
{
    Q_OBJECT

public:
    void loadEntry(PlaylistEntry* entry);
    void setDataAndSave(int type, int device, int channel, QString color, QString fontColor);
    static PlaylistPlayer* getObjectWithNumber(int number);
    static QMap<int,PlaylistPlayer*> getPlayers();
    static void fadeOutAllPlayers();
    PlaylistEntry* getLoadedEntry();

private:
    PlaylistPlayer(int number, QObject *parent = 0);
    PlaylistEntry *loadedEntry;
    static QMap<int,PlaylistPlayer*> audioObjects;

public slots:
    void play();
    void stop();

private slots:
    void readData();
    void saveData();
    void updatePosition();

signals:
    void entryLoaded(PlaylistEntry*);
    void started(PlaylistEntry*);
    void stopped(PlaylistEntry*);
    void reachedFadePosition(int player);

};

#endif // PLAYLISTPLAYER_H
