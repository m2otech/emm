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

#include "bassdevice.h"
#include "bassmix.h"
#include "model/configuration.h"
#include "view/mainwindow.h"

QList<BassDevice*> BassDevice::allDevices = QList<BassDevice*>();

BassDevice::BassDevice(int deviceID, QObject *parent) :
        AbstractBassDevice(deviceID, parent)
{

}

QString BassDevice::getName()
{
    return this->deviceInfo.name;
}

void BassDevice::init()
{
    if (!BASS_Init(deviceID,44100,0,(HWND)mainwindow->winId(),NULL)) {
        emit errorOccured(BASS_ErrorGetCode());
    }
    else
    {
        this->initialized = true;
        if (!this->setOutputDevice())
            return;

        channelCount = getInfo().speakers/2;
        for (int a=0;a<channelCount;a++) {
            HSTREAM newMixer = BASS_Mixer_StreamCreate(44100,2, BASS_SAMPLE_FLOAT | getSpeakerFlag(a+1));
            streams.append(newMixer);
            BASS_ChannelPlay(newMixer,false);
        }
    }
}

BASS_INFO BassDevice::getInfo()
{
    this->setOutputDevice();
    BASS_INFO info;
    if (!BASS_GetInfo(&info))
    {
        emit errorOccured(BASS_ErrorGetCode());
    }
    return info;
}

bool BassDevice::setOutputDevice()
{
    if (!BASS_SetDevice(deviceID))
    {
        emit errorOccured(BASS_ErrorGetCode());
        return false;
    }
    return true;
}

void BassDevice::free()
{
    this->setOutputDevice();
    if (!BASS_Free())
    {
        emit errorOccured(BASS_ErrorGetCode());
    }
    else
    {
        this->initialized = false;
    }
}

void BassDevice::setBuffer(int ms)
{
    foreach(BassDevice* device, allDevices)
    {
        if (device->isInitialized()) {
            device->setOutputDevice();
            BASS_SetConfig(BASS_CONFIG_BUFFER,ms);
        }
    }
}

void BassDevice::initialize(MainWindow *mw)
{
    mainwindow = mw;
    int a;
    BASS_DEVICEINFO info;
    BASS_SetConfig(BASS_CONFIG_BUFFER,Configuration::getInstance()->getSlotBuffer());

    for (a=0; BASS_GetDeviceInfo(a, &info); a++) {
        if (info.flags&BASS_DEVICE_ENABLED) {
            BassDevice *newDevice = new BassDevice(a,mw);
            newDevice->deviceInfo = info;
            allDevices.append(newDevice);
            connect(newDevice,SIGNAL(errorOccured(int)),mw,SLOT(showBassErrorMessage(int)));
            //newDevice->init();
        }
    }
}

DWORD BassDevice::getSpeakerFlag(int channel)
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

QList<BassDevice*> BassDevice::getAllDevices()
{
    return allDevices;
}

BassDevice* BassDevice::getDeviceWithID(int id)
{
    foreach(BassDevice *device, allDevices)
    {
        if (device->deviceID == id) return device;
    }
    return NULL;
}

void BassDevice::freeAllDevices()
{
    foreach(BassDevice *device, allDevices)
    {
        if (device->initialized)
            device->free();
    }
}

