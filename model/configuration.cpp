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
#include "configuration.h"
#include "layerdata.h"
#include "model/audio/bassdevice.h"
#include "model/audio/bassasiodevice.h"

#include "view/mainwindow.h"

extern bool DEMO;

Configuration* Configuration::instance = 0;

int Configuration::getHorizontalSlots()
{
    return this->horizontalSlots;
}

int Configuration::getVerticalSlots()
{
    return this->verticalSlots;
}

int Configuration::getPFLDriver()
{
    return this->pflDriver;
}

int Configuration::getPFLDevice()
{
    return this->pflDevice;
}

int Configuration::getPFLChannel()
{
    return this->pflChannel;
}

int Configuration::getDefaultDriver()
{
    return this->defaultDriver;
}

int Configuration::getDefaultDevice()
{
    return this->defaultDevice;
}

int Configuration::getDefaultChannel()
{
    return this->defaultChannel;
}

int Configuration::getPlayer()
{
    return this->player;
}

int Configuration::getPlaylistFade()
{
    return this->playlistFade;
}

int Configuration::getSlotFade()
{
    return this->slotFade;
}

int Configuration::getPlaylistFPos()
{
    return this->playlistFPos;
}

int Configuration::getLayer()
{
    return this->layer;
}



int Configuration::getSlotBuffer()
{
    return this->slotBuffer;
}

bool Configuration::getLayerKeyboardSync()
{
    return this->layerKeyboardSync;
}

bool Configuration::getPauseButton()
{
    return this->pauseButton;
}

int Configuration::getSlotTimeSize() {
    return this->slotTimeSize;
}

QMap<int, LayerData*> Configuration::getLayers() {
    return this->layers;
}

void Configuration::setHorizontalSlots(int horizontalSlots)
{
    this->horizontalSlots = horizontalSlots;
}

void Configuration::setVerticalSlots(int verticalSlots)
{
    this->verticalSlots = verticalSlots;
}

void Configuration::setPFLDriver(int pflDriver)
{
    this->pflDriver = pflDriver;
}

void Configuration::setPFLDevice(int pflDevice)
{
    this->pflDevice = pflDevice;
}

void Configuration::setPFLChannel(int pflChannel)
{
    this->pflChannel = pflChannel;
}

void Configuration::setDefaultDriver(int defaultDriver)
{
    this->defaultDriver = defaultDriver;
}

void Configuration::setDefaultDevice(int defaultDevice)
{
    this->defaultDevice = defaultDevice;
}

void Configuration::setDefaultChannel(int defaultChannel)
{
    this->defaultChannel = defaultChannel;
}

void Configuration::setPlayer(int player)
{
    this->player = player;
}

void Configuration::setPlaylistFade(int playlistFade)
{
    this->playlistFade = playlistFade;
}

void Configuration::setSlotFade(int slotFade)
{
    this->slotFade = slotFade;
}

void Configuration::setPlaylistFPos(int playlistFPos)
{
    this->playlistFPos = playlistFPos;
}

void Configuration::setLayer(int layer)
{
    this->layer = layer;
}

void Configuration::setSlotBuffer(int slotBuffer)
{
    this->slotBuffer = slotBuffer;
}

void Configuration::setLayerKeyboardSync(bool layerKeyboardSync)
{
    this->layerKeyboardSync = layerKeyboardSync;
}

void Configuration::setPauseButton(bool pauseButton)
{
    this->pauseButton = pauseButton;
}

void Configuration::setSlotTimeSize(int size) {
    this->slotTimeSize = size;
}

void Configuration::readData()
{
    QSettings settings(Configuration::getStorageLocation() + "/config.ini", QSettings::IniFormat);
    pflDriver = settings.value("PFL/Type",0).toInt();
    pflDevice = settings.value("PFL/Device",1).toInt();
    pflChannel = settings.value("PFL/Channel",1).toInt();
    defaultDriver = settings.value("DefaultDevice/Type",0).toInt();
    defaultDevice = settings.value("DefaultDevice/Device",1).toInt();
    defaultChannel = settings.value("DefaultDevice/Channel",1).toInt();
    player = settings.value("Playlist/Player",2).toInt();
    playlistFade = settings.value("Playlist/Fade",4000).toInt();
    slotFade = settings.value("Slots/Fade",800).toInt();
    playlistFPos = settings.value("Playlist/FadePos",8000).toInt();
    slotBuffer = settings.value("Slots/Buffer",5000).toInt();
    layerKeyboardSync = settings.value("Slots/LayerKeyboardSync",false).toBool();
    pauseButton = settings.value("Slots/PauseButton",false).toBool();
    slotTimeSize = settings.value("Slots/TimeSize",10).toInt();

    // DEMO (7x7 slots)
    if (DEMO)
    {
        horizontalSlots = 7;
        verticalSlots = 7;
    }
    else
    {
        horizontalSlots = settings.value("Slots/Horizontal",5).toInt();
        verticalSlots = settings.value("Slots/Vertical",5).toInt();
    }

    // DEMO (1 layer only)    
    if (DEMO)
        layer = 1;
    else
        layer = settings.value("Slots/Layer",1).toInt();

    layers.clear();
    for (int i=0;i<layer;i++) {
        LayerData *data = new LayerData(i+1);
        if (DEMO)
        {
            data->setName(settings.value("Demo/Name","Demo").toString());
            data->setVisible(settings.value("Demo/Visible",true).toBool());
        }
        else
        {
            data->setName(settings.value("Layer"+QString::number(i+1)+"/Name","Layer "+QString::number(i+1)).toString());
            data->setVisible(settings.value("Layer"+QString::number(i+1)+"/Visible",true).toBool());
        }
        layers.insert(i,data);
    }
}

void Configuration::saveData()
{
    QSettings settings(Configuration::getStorageLocation() + "/config.ini", QSettings::IniFormat);
    settings.setValue("Slots/Horizontal",horizontalSlots);
    settings.setValue("Slots/Vertical",verticalSlots);
    settings.setValue("Slots/Layer",layer);
    settings.setValue("PFL/Type",pflDriver);
    settings.setValue("PFL/Device",pflDevice);
    settings.setValue("PFL/Channel",pflChannel);
    settings.setValue("DefaultDevice/Type",defaultDriver);
    settings.setValue("DefaultDevice/Device",defaultDevice);
    settings.setValue("DefaultDevice/Channel",defaultChannel);
    settings.setValue("Playlist/Player",player);
    settings.setValue("Playlist/Fade",playlistFade);
    settings.setValue("Slots/Fade",slotFade);
    settings.setValue("Playlist/FadePos",playlistFPos);
    settings.setValue("Slots/Buffer",slotBuffer);
    settings.setValue("Slots/LayerKeyboardSync",layerKeyboardSync);
    settings.setValue("Slots/TimeSize",slotTimeSize);
    settings.setValue("Slots/PauseButton",pauseButton);
    BassDevice::setBuffer(slotBuffer);
    BassAsioDevice::setBuffer(slotBuffer);

    for (int i=0;i<layer;i++) {
        LayerData *data = layers.value(i);
        settings.setValue("Layer"+QString::number(i+1)+"/Name",data->getName());
        settings.setValue("Layer"+QString::number(i+1)+"/Visible",data->getVisible());
    }

    // m2: Re-allocate array holding slot counters and pause info
    MainWindow *currInstance = MainWindow::getInstance();

    int numberOfSlotsOld = currInstance->numberOfSlots;
    int numberOfSlotsNew = layer * horizontalSlots * verticalSlots;

    int *tmp = NULL;
    int *tmp2 = NULL;

    tmp = (int *) realloc((void*) currInstance->playedCtr, numberOfSlotsNew * sizeof(int));
    tmp2 = (int *) realloc((void*) currInstance->pausedSlot, numberOfSlotsNew * sizeof(int));

    if ( ( ! (tmp == NULL) ) && ( ! (tmp2 == NULL) ) ) {
        currInstance->playedCtr = tmp;
        currInstance->pausedSlot = tmp2;

        // fill extra slots with 0
        if ( numberOfSlotsNew > numberOfSlotsOld ) {
            memset(&currInstance->playedCtr[numberOfSlotsOld], 0, (numberOfSlotsNew - numberOfSlotsOld) * sizeof(int));
            memset(&currInstance->pausedSlot[numberOfSlotsOld], 0, (numberOfSlotsNew - numberOfSlotsOld) * sizeof(int));
        }

        currInstance->numberOfSlots = numberOfSlotsNew;
    }

    // DEBUG
//    for (int i = 0; i < numberOfSlotsNew; i++)
//    {
//        qDebug("slot[%d] = %d", i, currInstance->playedCtr[i]);
//    }
//    qDebug("no of layers %d", layer);

}

void Configuration::updateLayerCount(int count) {
    if (count > layers.size()) {
        LayerData *layer = new LayerData(count);
        layer->setName("Layer "+QString::number(count));
        layer->setVisible(true);
        layers.insert(count-1,layer);
    } else {
        layers.remove(count);
    }
}

Configuration* Configuration::getInstance()
{
    static QMutex mutex;
    if (!instance)
    {
        mutex.lock();

        if (!instance)
            instance = new Configuration;
        instance->readData();

        mutex.unlock();
    }
    return instance;
}

void Configuration::dropInstance()
{
    static QMutex mutex;
    mutex.lock();
    delete instance;
    instance = 0;
    mutex.unlock();
}

QString Configuration::getStorageLocation()
{
    QSettings settings("Christoph Krämer","EventMusicMachine");
    return settings.value("configPath","").toString();
}

void Configuration::setStorageLocation(QString location)
{
    QSettings settings("Christoph Krämer","EventMusicMachine");
    settings.setValue("configPath",location);
}
