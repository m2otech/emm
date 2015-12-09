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

#ifndef BASSASIODEVICE_H
#define BASSASIODEVICE_H

#include "abstractbassdevice.h"
#include "bassasio.h"

class BassAsioDevice : public AbstractBassDevice
{
    Q_OBJECT
public:
    QString getName();
    explicit BassAsioDevice(int deviceID, QObject *parent = 0);
    static void initialize(MainWindow *mw);
    static QList<BassAsioDevice*> getAllDevices();
    static BassAsioDevice* getDeviceWithID(int id);
    static void freeAllDevices();
    static void setBuffer(int ms);
    static DWORD CALLBACK proc(BOOL input, DWORD channel, void *buffer, DWORD length, void *user);
    void init();
    bool setOutputDevice();

private:
    BASS_ASIO_DEVICEINFO deviceInfo;
    static QList<BassAsioDevice*> allDevices;

    BASS_ASIO_INFO getInfo();
    void free();

};

#endif // BASSASIODEVICE_H
