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

#include <QMutex>
#include "configuration.h"
#include "model/audio/playlistplayer.h"
#include "playlistentry.h"
#include "playlist.h"

#include "view/mainwindow.h"

Playlist* Playlist::instance = 0;



//PlaylistPlayer *player = PlaylistPlayer::getObjectWithNumber(playerNumber);
//player->setDataAndSave(
//        ui->deviceSelectWidget->getDriver(),
//        ui->deviceSelectWidget->getDevice(),
//        ui->deviceSelectWidget->getChannel(),
//        color,
//        fontColor
//        );

PlaylistEntry* Playlist::addEntry(QString filename, int pos)
{
    PlaylistEntry *newEntry = new PlaylistEntry(filename);
    if (pos == -1)
        entries.append(newEntry);
    else
        entries.insert(pos,newEntry);

    this->assignEntriesToPlayers();

    // m2: Update TOT LENGTH
    double remaining = this->getTotalLength();
    int mins2 = remaining/60;
    int secs2 = floor(remaining-mins2*60);
    int msecs2 = floor((remaining-mins2*60-secs2)*10);
    QString time = QString("TOT %1:%2.%3").arg(mins2, 2, 10, QChar('0')).arg(secs2,2,10, QChar('0')).arg(msecs2);

    if (time.size() > 0)
        MainWindow::getInstance()->setInfoBox_2(time);

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

//m2: total playlist length
double Playlist::getTotalLength()
{
    double sum = 0;
    for (int i = 0; i < entries.size(); i++)
        sum += entries.at(i)->getLength();

    //qDebug() << QString("Total length: %1").arg(sum);
    return sum;
}

//m2: playlist length to play
double Playlist::getRemainingLength()
{
    double sum = 0;
    int curr = 0;
    for (int i = 0; i < entries.size(); i++)
        if (playing.contains(entries.at(i)))
            curr = i;
    for (int i = curr + 1; i < entries.size(); i++)
        sum += entries.at(i)->getLength();

    //qDebug() << QString("Remaining length: %1").arg(sum);
    return sum;
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

// m2: play one player if no player playing (otherwise just stop the playing player)
void Playlist::playPlayer(int player)
{
    bool stoppedNow = false;
    for (int i = 1; i <= PlaylistPlayer::getPlayers().size(); i++) {
        if (PlaylistPlayer::getObjectWithNumber(i)->isPlaying()) {
            int fadeMs = Configuration::getInstance()->getPlaylistFade();
            PlaylistPlayer::getObjectWithNumber(i)->fadeOut(fadeMs);
            stoppedNow = true;
        }
    }
    if (!stoppedNow)
        PlaylistPlayer::getObjectWithNumber(player)->play();
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
            if (players2.value()->getLoadedEntry() != NULL) {
                players2.value()->play();
            }
            break;
        }
    }
    int ms = Configuration::getInstance()->getPlaylistFade();
    QMapIterator<int, PlaylistPlayer*> players(PlaylistPlayer::getPlayers());
    while (players.hasNext()) {
        players.next();
        if (players.key() == started)
            continue;
        if (players.value()->isPlaying()) {
            players.value()->fadeOut(ms);
        }
    }
    this->getRemainingLength();
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
