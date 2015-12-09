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

#include <QTimer>
#include <math.h>
#include "abstractaudioobject.h"
#include "bassmix.h"
#include "bass_fx.h"
#include "model/audio/bassasiodevice.h"
#include "model/audio/bassdevice.h"
#include "model/keyboardcontroller.h"

AbstractAudioObject::AbstractAudioObject(int, QObject *parent) : QObject(parent)
{
    playing = false;
    paused = false;
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updatePosition()));
}

int AbstractAudioObject::getDeviceID()
{
    return this->device;
}

int AbstractAudioObject::getChannel()
{
    return this->channel;
}

int AbstractAudioObject::getType()
{
    return this->type;
}

QString AbstractAudioObject::getFileName()
{
    return this->filename;
}

void AbstractAudioObject::loadStream(double vol)
{
    switch (type)
    {
    case 0:
        BASS_SetDevice(device);
        break;
    case 1:
        BASS_SetDevice(0);
        BASS_ASIO_SetDevice(device);
        break;
    }

    unloadStream();
    stream = BASS_StreamCreateFile(false,filename.toLatin1(),0,0, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE);
    if (BASS_ErrorGetCode() == 2 && filename != "") {
        fileMissing = true;
    } else {
        fileMissing = false;
    }
    if (type == 0 && device == 0)
        return;

    switch (type)
    {
    case 0:
        stream = BASS_FX_TempoCreate(stream,BASS_FX_FREESOURCE);
        if (BassDevice::getDeviceWithID(device) == NULL) return;
        BassDevice::getDeviceWithID(device)->addStreamToMixer(stream,channel);
        break;
    case 1:
        stream = BASS_FX_TempoCreate(stream,BASS_FX_FREESOURCE | BASS_STREAM_DECODE);
        if (BassAsioDevice::getDeviceWithID(device) == NULL) return;
        BassAsioDevice::getDeviceWithID(device)->addStreamToMixer(stream,channel);
        break;
    }
    if (vol != 0) {
        HFX hstream = BASS_ChannelSetFX(stream, BASS_FX_BFX_VOLUME,0);
        BASS_BFX_VOLUME volume;
        volume.fVolume = DBToLevel(vol);
        volume.lChannel = 0;
        BASS_FXSetParameters(hstream,&volume);
    }
    for(int i=0; i<eqs.length();i++) {
        HFX hstream = BASS_ChannelSetFX(stream, BASS_FX_DX8_PARAMEQ,0);
        BASS_FXSetParameters(hstream,&eqs.at(i));
    }
}

DWORD AbstractAudioObject::getSpeakerFlag()
{
    switch (channel)
    {
    case 1: return BASS_SPEAKER_FRONT;
    case 2: return BASS_SPEAKER_CENLFE;
    case 3: return BASS_SPEAKER_REAR;
    case 4: return BASS_SPEAKER_REAR2;
    }
    return BASS_SPEAKER_FRONT;
}

void AbstractAudioObject::play()
{
    KeyboardController::increasePlayerCount();
    BASS_Mixer_ChannelFlags(stream, 0, BASS_MIXER_PAUSE);
    BASS_ChannelSetAttribute(stream,BASS_ATTRIB_VOL,1);
    timer->start(100);
    BASS_ChannelPlay(stream,true);
    emit sendSongLength(BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetLength(stream,BASS_POS_BYTE)));
    playing = true;
    paused = false;
    emit started();
}

void AbstractAudioObject::stop()
{
    if (!playing)
        return;
    KeyboardController::reducePlayerCount();
    BASS_ChannelStop(stream);
    BASS_Mixer_ChannelFlags(stream, BASS_MIXER_PAUSE, BASS_MIXER_PAUSE);
    playing = false;
    emit stopped();
    timer->stop();
    unloadStream();
}

void AbstractAudioObject::pause()
{
    if (playing)
    {
        //qDebug("Pause called");
        BASS_ChannelPause(stream);
        BASS_Mixer_ChannelFlags(stream, BASS_MIXER_PAUSE, BASS_MIXER_PAUSE);
        playing = false;
        paused = true;
        emit pausedSignal();
    }
    else
    {
        //qDebug("Pause (restart) called");
        BASS_Mixer_ChannelFlags(stream, 0, BASS_MIXER_PAUSE);
        BASS_ChannelSetAttribute(stream,BASS_ATTRIB_VOL,1);
        BASS_ChannelPlay(stream,false);
        playing = true;
        paused = false;
    }
}
double AbstractAudioObject::getTimeLeft()
{
    return BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetLength(stream,BASS_POS_BYTE)-BASS_ChannelGetPosition(stream,BASS_POS_BYTE));
}

bool AbstractAudioObject::isPlaying()
{
    return this->playing;
}

bool AbstractAudioObject::isPaused()
{
    return this->paused;
}

void AbstractAudioObject::unloadStream()
{
    BASS_StreamFree(stream);
}

void AbstractAudioObject::fadeOut(int ms)
{
    BASS_ChannelSlideAttribute(stream, BASS_ATTRIB_VOL, -1, ms);
    QTimer *test = new QTimer(this);
    test->singleShot(ms,this,SLOT(stop()));
}

void AbstractAudioObject::fadeOutPause(int ms)
{
    BASS_ChannelSlideAttribute(stream, BASS_ATTRIB_VOL, -1, ms);
    QTimer *test = new QTimer(this);
    test->singleShot(ms,this,SLOT(pause()));
}

QString AbstractAudioObject::getColor()
{
    return this->color;
}

QString AbstractAudioObject::getFontColor()
{
    if (this->fontColor == "0")
        return "#ffffff";
    return this->fontColor;
}

void AbstractAudioObject::updatePosition()
{
    QWORD pos = BASS_ChannelGetPosition(stream,BASS_POS_BYTE);
    double pos2 = BASS_ChannelBytes2Seconds(stream,pos);
    emit sendCurrentPosition(pos2);
    if (BASS_ChannelGetPosition(stream,BASS_POS_BYTE)==BASS_ChannelGetLength(stream,BASS_POS_BYTE))
    {
        this->stop();
        //return 0;
    }
}

void AbstractAudioObject::initDevice()
{
    switch (type)
    {
    case 0: {
            BassDevice *bd = BassDevice::getDeviceWithID(device);
            if (bd == NULL || device == 0)
                return;
            if (!bd->isInitialized())
                bd->init();
        }
        break;
    case 1: {
            BassAsioDevice *bd = BassAsioDevice::getDeviceWithID(device);
            if (bd == NULL)
                return;
            if (!bd->isInitialized())
                bd->init();
        }
        break;
    }
}

double AbstractAudioObject::DBToLevel(double db)
{
    return pow(10,db/20);
}

bool AbstractAudioObject::isMissing() {
    return this->fileMissing;
}
