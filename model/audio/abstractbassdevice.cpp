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

#include "abstractbassdevice.h"
#include "bassmix.h"
#include "view/mainwindow.h"

MainWindow* AbstractBassDevice::mainwindow = NULL;

AbstractBassDevice::AbstractBassDevice(int deviceID, QObject *parent) :
        QObject(parent)
{
    this->deviceID = deviceID;
    this->initialized = false;
    this->channelCount = 0;
}

void AbstractBassDevice::addStreamToMixer(HSTREAM stream, int channel, bool free)
{
    if (channel>streams.size()) return;
    if (free)
        BASS_Mixer_StreamAddChannel(streams.at(channel-1),stream, BASS_MIXER_PAUSE | BASS_STREAM_AUTOFREE);
    else
        BASS_Mixer_StreamAddChannel(streams.at(channel-1),stream, BASS_MIXER_PAUSE);
}

int AbstractBassDevice::getDeviceID()
{
    return this->deviceID;
}

int AbstractBassDevice::getChannelCount()
{
    if (!this->initialized) {
        this->init();
    }
    return this->channelCount;
}

bool AbstractBassDevice::isInitialized()
{
    return this->initialized;
}
