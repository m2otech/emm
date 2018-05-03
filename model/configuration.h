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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QMap>
#include <QObject>

class LayerData;

class Configuration : public QObject
{
    Q_OBJECT
public:
    int getHorizontalSlots();
    int getVerticalSlots();
    int getPFLDriver();
    int getPFLDevice();
    int getPFLChannel();
    int getDefaultDriver();
    int getDefaultDevice();
    int getDefaultChannel();
    int getPlayer();
    int getPlaylistFade();
    int getSlotFade();
    int getPlaylistFPos();
    int getLayer();
    int getSlotBuffer();
    bool getLayerKeyboardSync();
    bool getPitchKeyboard();
    bool getPauseButton();
    int getSlotTimeSize();
    QMap<int, LayerData*> getLayers();

    // m2: Options for layer bar
    bool getLayerbarPauseButton();
    bool getLayerbarStopButton();
    bool getLayerbarPitchControl();
    bool getLayerbarRLADisplay();
    bool getLayerbarPlaylistButton();

    void setHorizontalSlots(int horizontalSlots);
    void setVerticalSlots(int verticalSlots);
    void setPFLDriver(int pflDriver);
    void setPFLDevice(int pflDevice);
    void setPFLChannel(int pflChannel);
    void setDefaultDriver(int defaultDriver);
    void setDefaultDevice(int defaultDevice);
    void setDefaultChannel(int defaultChannel);
    void setPlayer(int player);
    void setPlaylistFade(int playlistFade);
    void setSlotFade(int slotFade);
    void setPlaylistFPos(int playlistFPos);
    void setLayer(int layer);
    void setSlotBuffer(int slotBuffer);
    void setLayerKeyboardSync(bool layerKeyboardSync);
    void setPitchKeyboard(bool);
    void setPauseButton(bool pauseButton);

    // m2: Options for layer bar
    void setLayerbarPauseButton(bool);
    void setLayerbarStopButton(bool);
    void setLayerbarPitchControl(bool);
    void setLayerbarRLADisplay(bool);
    void setLayerbarPlaylistButton(bool);

    void setSlotTimeSize(int size);
    void updateLayerCount(int newCount);
    void saveData();
    static void setStorageLocation(QString location);
    static QString getStorageLocation();
    static Configuration* getInstance();
    static void dropInstance();

private:
    Configuration() {}
    Configuration(const Configuration &);
    Configuration& operator=(const Configuration &);
    static Configuration* instance;

    int horizontalSlots;
    int verticalSlots;
    int layer;
    int pflDriver;
    int pflDevice;
    int pflChannel;
    int defaultDriver;
    int defaultDevice;
    int defaultChannel;
    int player;
    int playlistFade;
    int playlistFPos;
    int slotFade;
    int slotBuffer;
    int slotTimeSize;
    bool layerKeyboardSync;
    bool pauseButton;
    bool pitchKeyboard;
    QMap<int, LayerData*> layers;

    bool layerbarShowPause;
    bool layerbarShowStop;
    bool layerbarShowPitch;
    bool layerbarShowPlaylist;
    bool layerbarShowRLA;

    void readData();
};

#endif // CONFIGURATION_H
