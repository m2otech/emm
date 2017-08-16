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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>

#include "slotstoredialog.h"

namespace Ui {
    class MainWindow;
}
class PlayerWidget;
class QFileSystemModel;
class LoadSlotThread;
class QNetworkReply;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();

    // m2: array pointer to hold slot counters
    int *playedCtr;
    // m2: dynamically changing number of slots (adding/removing layers or changing layout)
    int numberOfSlots;
    // m2: information which title is currently paused (via keyboard pause function)
    int *pausedSlot;

    // m2: increase slot counter
    void increaseCounter(int);

    // m2: hold state variables
    static MainWindow* getInstance();
    static void dropInstance();

    // m2: get slots from all layers (visible and invisible)
    int getTotalNumberOfSlots();

    // m2: reset pause info (when playing other slot)
    void resetPause();

    // m2: change back to previously selected layer (when keyboard controls first layer)
    void resetLayer();

    // m2: layer that was shown before changing to layer 1 via keyboard
    int prevLayer;

    // m2: set text to box right of layer bar
    void setInfoBox(QString);

    // m2: methods for RLA
    void updateCurrSongPosition(double, int);
    void infoBoxAddToQueue(int);
    void infoBoxRemoveFromQueue(int);
    int infoBoxGetLast();

    // m2: change pitch display when starting slot
    void pitchDisplayUpdate(int);

    bool isSlotShown(int);

protected:
    void wheelEvent(QWheelEvent *);

private:
    Ui::MainWindow *ui;
    QList<PlayerWidget*> playerWidgets;
    QFileSystemModel *fileModel;
    LoadSlotThread *slotThread;
    bool pauseModifier;

    // m2: added to hold state variables
    static MainWindow* instance;

    SlotStoreDialog* ssdGlobal;

    // m2: managing RLA
    QList<int> infoBoxQueue;

    void closeEvent (QCloseEvent *);

private slots:
    void showConfigDialog();
    void showErrorMessage(QString);
    void showBassErrorMessage(int code);
    void createPlayers();
    void updateSlotAssignment();
    void keyboardSignal(int key, int pressed);
    void setAutoPlay();
    void showAbout();
    void showSlotStore();
    void copyColors();
    void clearSlots();
    void setConfigDirectory();
    // m2: pause/stop running slots
    void pauseSlots();
    void stopSlots();
    // m2: pitch change while playing
    void pitchUp();
    void pitchDown();
    void pitchReset();
    void pitchChange(int);
    int getLayerFirstSlotId();
    int getLayerNumberOfSlots();
    int getCurrentLayer();
    int getSlotLayer(int);

    // m2: reset all slot counters to 0
    void resetCounters(bool);
    void resetCounters();

    void gotoLayer(int);

    void showPlayer();
    void showSlots();

};

#endif // MAINWINDOW_H
