﻿/* ***************************************************************************
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

#include <QSettings>
#include "bassasiodevice.h"
#include "bassdevice.h"
#include "cartslot.h"
#include "model/playlistentry.h"
#include "model/configuration.h"
#include "model/playlist.h"
#include "playlistplayer.h"

#include "view/editplayerdialog.h"

#include "view/mainwindow.h"

QMap<int,PlaylistPlayer*> PlaylistPlayer::audioObjects = QMap<int,PlaylistPlayer*>();

PlaylistPlayer::PlaylistPlayer(int number, QObject *parent) :
        AbstractAudioObject(number, parent)
{
    this->number = number;
    this->loadedEntry = NULL;
    readData();
    initDevice();
    audioObjects.insert(number,this);
    connect(this, SIGNAL(started(PlaylistEntry*)), Playlist::getInstance(), SLOT(addItemToPlaying(PlaylistEntry*)));
    connect(this, SIGNAL(stopped(PlaylistEntry*)), Playlist::getInstance(), SLOT(removeItemFromPlaylist(PlaylistEntry*)));
    connect(this, SIGNAL(reachedFadePosition(int)), Playlist::getInstance(), SLOT(doAutoPlay(int)));

    playerQueue.append(-1000);
}

void PlaylistPlayer::setDataAndSave(int type, int device, int channel, QString color, QString fontColor)
{
    this->device = device;
    this->channel = channel;
    this->type = type;
    this->color = color;
    this->fontColor = fontColor;
    this->saveData();
}

void PlaylistPlayer::readData()
{
    // m2: Default colors when no entry in slots.ini

    QString fontColorDef = "#000000";
    QString colorDef = "#FFFFFF";

    // PLAYER 1
    if (this->number == 1) {
        fontColorDef = "#000000";
        colorDef = "#FFFFFF";
    }

    // PLAYER 2
    else if (this->number == 2) {
        fontColorDef = "#FFFFFF";
        colorDef = "#000000";
    }

    QSettings settings(Configuration::getStorageLocation() + "/slots.ini", QSettings::IniFormat);
    type = settings.value("Player"+QString::number(number)+"/Type",0).toInt();
    device = settings.value("Player"+QString::number(number)+"/Device",1).toInt();
    channel = settings.value("Player"+QString::number(number)+"/Channel",1).toInt();
    color = settings.value("Player"+QString::number(number)+"/Color",colorDef).toString();
    fontColor = settings.value("Player"+QString::number(number)+"/FontColor",fontColorDef).toString();
}

void PlaylistPlayer::saveData()
{
    QSettings settings(Configuration::getStorageLocation() + "/slots.ini", QSettings::IniFormat);
    settings.setValue("Player"+QString::number(number)+"/Type",type);
    settings.setValue("Player"+QString::number(number)+"/Device",device);
    settings.setValue("Player"+QString::number(number)+"/Channel",channel);
    settings.setValue("Player"+QString::number(number)+"/Color",color);
    settings.setValue("Player"+QString::number(number)+"/FontColor",fontColor);
}

PlaylistPlayer* PlaylistPlayer::getObjectWithNumber(int number)
{
    if (audioObjects.contains(number))
        return audioObjects.value(number);
    else
        return new PlaylistPlayer(number);
}

void PlaylistPlayer::play()
{
    // m2: add to RLA (Playlist players have number -1001 and -1002)
    MainWindow::getInstance()->infoBoxAddToQueue(-1000 - this->number);

    AbstractAudioObject::play();
    fading = false;
    emit started(loadedEntry);
    CartSlot::fadeOutAllSlots();
}

void PlaylistPlayer::stop()
{
    //AbstractAudioObject::stop();

    // m2: remove from RLA (Playlist players have number -1001 and -1002)
    MainWindow::getInstance()->infoBoxRemoveFromQueue(-1000 - this->number);

    AbstractAudioObject::stop();
    if (loadedEntry != NULL)
        emit stopped(loadedEntry);
}

void PlaylistPlayer::loadEntry(PlaylistEntry *entry)
{
    loadedEntry = entry;
    if (entry == NULL)
    {
        this->unloadStream();
        this->filename = "";
    }
    else
    {
        this->filename = loadedEntry->getFilename();
        this->loadStream();
    }
    emit entryLoaded(entry);
}

void PlaylistPlayer::updatePosition()
{
    //AbstractAudioObject::updatePosition();

    // m2: Update RLA (Playlist players have number -1001 and -1002)
    int currPlayerNo = -1000 - this->number;
    AbstractAudioObject::updatePosition(currPlayerNo, currPlayerNo);

    int ms = Configuration::getInstance()->getPlaylistFPos();
    if (!fading && BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetLength(stream,BASS_POS_BYTE)-BASS_ChannelGetPosition(stream,BASS_POS_BYTE))*1000 <= ms)
    {
        emit reachedFadePosition(this->number);
        this->fading = true;
    }
}

QMap<int,PlaylistPlayer*> PlaylistPlayer::getPlayers()
{
    return audioObjects;
}

PlaylistEntry* PlaylistPlayer::getLoadedEntry()
{
    return loadedEntry;
}

void PlaylistPlayer::fadeOutAllPlayers()
{

    int fadeMs = Configuration::getInstance()->getPlaylistFade();
    QMapIterator<int, PlaylistPlayer*> j(PlaylistPlayer::getPlayers());
    while (j.hasNext()) {
        j.next();
        PlaylistPlayer *slot = j.value();
        if (slot->isPlaying())
            slot->fadeOut(fadeMs);
    }
}
