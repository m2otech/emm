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
#include <QSettings>
#include <math.h>
#include "model/configuration.h"
#include "pflplayer.h"
#include "tags.h"

PFLPlayer* PFLPlayer::instance = 0;

PFLPlayer::PFLPlayer(int number, QObject *parent) :
        AbstractAudioObject(number, parent)
{
    readData();
    initDevice();
}

void PFLPlayer::readData()
{
    QSettings settings(Configuration::getStorageLocation() + "/config.ini", QSettings::IniFormat);
    device = settings.value("PFL/Device",0).toInt();
    channel = settings.value("PFL/Channel",0).toInt();
    type = settings.value("PFL/Type",0).toInt();
}

void PFLPlayer::analyse(bool readName)
{
    BASS_SetDevice(0);
    peak=0;
    visualInformation.clear();
    HSTREAM channel=BASS_StreamCreateFile(FALSE,this->filename.toLatin1(),0,0,BASS_STREAM_DECODE);
    if (!channel) return;
    while (BASS_ChannelIsActive(channel)) { // not reached end, keep scanning...
        DWORD level=BASS_ChannelGetLevel(channel);
        if (peak<LOWORD(level)) peak=LOWORD(level);
        if (peak<HIWORD(level)) peak=HIWORD(level);
        VolumeInformation vol;
        vol.left = LOWORD(level);
        vol.right = HIWORD(level);
        visualInformation.append(vol);
    }
    BASS_StreamFree(channel);
    emit newData(visualInformation);
    if (readName) {
        BASS_SetDevice(1);
        HSTREAM stream = BASS_StreamCreateFile(false,filename.toLatin1(),0,0,0);
        QString title = TAGS_Read(stream,"%TITL");
        if (title != "")
            emit sendName(title);
        BASS_StreamFree(stream);
    }
}

void PFLPlayer::setDB(double db)
{
    this->db = db;
}

void PFLPlayer::play() {
    this->loadStream(db);
    AbstractAudioObject::play();
}

void PFLPlayer::playCue(QTime time,bool end) {
    this->play();
    double startPos = time.minute()*60+time.second()+(double)time.msec()/1000;
    if (end)
        startPos = BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetLength(stream,BASS_POS_BYTE)) - startPos;
    BASS_ChannelSetPosition(stream,BASS_ChannelSeconds2Bytes(stream,startPos),BASS_POS_BYTE);
}

void PFLPlayer::setPosition(double pos) {
    if (!this->playing)
        return;
    BASS_ChannelSetPosition(stream,BASS_ChannelSeconds2Bytes(stream,pos),BASS_POS_BYTE);
}

QList<PFLPlayer::VolumeInformation> PFLPlayer::getVisualData()
{
    return visualInformation;
}

int PFLPlayer::getPeak()
{
    return peak;
}

double PFLPlayer::getPeakInDB()
{
    return 20 * log10((double)peak / (double)32768);
}

void PFLPlayer::setFilename(QString filename)
{
    this->filename = filename;
}

PFLPlayer* PFLPlayer::getInstance()
{
    static QMutex mutex;
    if (!instance)
    {
        mutex.lock();

        if (!instance) {
            instance = new PFLPlayer(0);
        }

        mutex.unlock();
    }
    return instance;
}

void PFLPlayer::dropInstance()
{
    static QMutex mutex;
    mutex.lock();
    delete instance;
    instance = 0;
    mutex.unlock();
}
