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

#include <QApplication>
#include "bassasiodevice.h"
#include "bassmix.h"
#include "model/configuration.h"
#include "view/mainwindow.h"

QList<BassAsioDevice*> BassAsioDevice::allDevices = QList<BassAsioDevice*>();

BassAsioDevice::BassAsioDevice(int deviceID, QObject *parent) :
        AbstractBassDevice(deviceID, parent)
{

}

QString BassAsioDevice::getName()
{
    return this->deviceInfo.name;
}

void BassAsioDevice::init()
{
    if (!BASS_ASIO_Init(deviceID, 0)) {
        emit errorOccured(BASS_ASIO_ErrorGetCode());
    }
    else
    {
        this->initialized = true;
        if (!this->setOutputDevice())
            return;
        channelCount = getInfo().outputs/2;

        for (int a=0;a<channelCount;a++) {
            HSTREAM newMixer = BASS_Mixer_StreamCreate(BASS_ASIO_GetRate(),2,BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
            streams.append(newMixer);
            BASS_ASIO_ChannelEnable(0,a*2,&proc,(void*)newMixer);
            BASS_ASIO_ChannelJoin(0,a*2+1,a*2);
            BASS_ASIO_ChannelSetFormat(0,a*2,BASS_ASIO_FORMAT_FLOAT);
        }
        BASS_ASIO_Start(0,0);
    }
}

BASS_ASIO_INFO BassAsioDevice::getInfo()
{
    this->setOutputDevice();
    BASS_ASIO_INFO info;
    if (!BASS_ASIO_GetInfo(&info))
    {
        emit errorOccured(BASS_ASIO_ErrorGetCode());
    }
    return info;
}

bool BassAsioDevice::setOutputDevice()
{
    if (!initialized) this->init();
    if (!BASS_ASIO_SetDevice(deviceID))
    {
        emit errorOccured(BASS_ASIO_ErrorGetCode());
        return false;
    }
    return true;
}

void BassAsioDevice::free()
{
    this->setOutputDevice();
    if (!BASS_ASIO_Free())
    {
        emit errorOccured(BASS_ASIO_ErrorGetCode());
    }
    else
    {
        this->initialized = false;
    }
}

void BassAsioDevice::initialize(MainWindow *mw)
{
    mainwindow = mw;
    BASS_Init(0, 48000, 0, 0, 0);
    BASS_SetDevice(0);
    BASS_SetConfig(BASS_CONFIG_BUFFER,Configuration::getInstance()->getSlotBuffer());

    int a;
    BASS_ASIO_DEVICEINFO info;

    for (a=0; BASS_ASIO_GetDeviceInfo(a, &info); a++) {
        BassAsioDevice *newDevice = new BassAsioDevice(a,mw);
        newDevice->deviceInfo = info;
        allDevices.append(newDevice);
        connect(newDevice,SIGNAL(errorOccured(int)),mw,SLOT(showBassErrorMessage(int)));
        //newDevice->init();
    }
}

DWORD CALLBACK BassAsioDevice::proc(BOOL input, DWORD channel, void *buffer, DWORD length, void *user)
{
    DWORD c=BASS_ChannelGetData((HSTREAM)user,buffer,length); // user = output #
    if (c==-1) c=0; // an error, no data
    return c;
}

QList<BassAsioDevice*> BassAsioDevice::getAllDevices()
{
    return allDevices;
}

BassAsioDevice* BassAsioDevice::getDeviceWithID(int id)
{
    foreach(BassAsioDevice *device, allDevices)
    {
        if (device->deviceID == id) return device;
    }
    return NULL;
}

void BassAsioDevice::freeAllDevices()
{
    foreach(BassAsioDevice *device, allDevices)
    {
        if (device->initialized)
            device->free();
    }
}

void BassAsioDevice::setBuffer(int ms)
{
    foreach(BassAsioDevice* device, allDevices)
    {
        if (device->isInitialized()) {
            device->setOutputDevice();
            BASS_SetConfig(BASS_CONFIG_BUFFER,ms);
        }
    }
}
