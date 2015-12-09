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
#include "audioprocessor.h"
#include "bassasiodevice.h"
#include "bassdevice.h"
#include "cartslot.h"
#include "pflplayer.h"
#include "playlistplayer.h"
#include "view/mainwindow.h"

AudioProcessor* AudioProcessor::instance = 0;

void AudioProcessor::initDevices(MainWindow *mw)
{
    BassDevice::initialize(mw);
    BassAsioDevice::initialize(mw);
}

void AudioProcessor::freeDevices()
{
    BassDevice::freeAllDevices();
    BassAsioDevice::freeAllDevices();
}

CartSlot* AudioProcessor::getCartSlotWithNumber(int number)
{
    return CartSlot::getObjectWithNumber(number);
}

PlaylistPlayer* AudioProcessor::getPlayerWithNumber(int number)
{
    return PlaylistPlayer::getObjectWithNumber(number);
}

PFLPlayer* AudioProcessor::getPFLPlayer()
{
    return PFLPlayer::getInstance();
}

void AudioProcessor::run()
{
    this->exec();
}

AudioProcessor* AudioProcessor::getInstance()
{
    static QMutex mutex;
    if (!instance)
    {
        mutex.lock();

        if (!instance) {
            instance = new AudioProcessor();
            instance->start();
        }

        mutex.unlock();
    }
    return instance;
}

void AudioProcessor::dropInstance()
{
    static QMutex mutex;
    mutex.lock();
    instance->freeDevices();
    instance->quit();
    instance->wait();
    delete instance;
    instance = 0;
    mutex.unlock();
}
