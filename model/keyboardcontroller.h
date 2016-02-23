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

#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

#include <QObject>
#include "MwxUsb.h"
#include "windows.h"

class QTimer;

class KeyboardController : public QObject
{
    Q_OBJECT
public:
    static KeyboardController* getInstance();
    static void dropInstance();
    int initializeKeyboardController();
    void closeKeyboardController();
    static void increasePlayerCount();
    static void reducePlayerCount();

private:
    KeyboardController() {}
    KeyboardController(const KeyboardController &);
    KeyboardController& operator=(const KeyboardController &);
    static KeyboardController* instance;

    static void __stdcall keyboardCallback(int value);
    static int keyCommandCount;
    static int playerCount;
    HINSTANCE hMWXUSB;
    OPEN_USB *lpopenusb;
    CLOSE_USB *lpcloseusb;
    SET_CALLBACK *lpcallback;
    READ_POS_KEY_USB *lpposkey;
    ACCEPT_LED *lpled;
    bool opened;
    int code;
    void sendKeyCommand();

signals:
    void keyPressed(int keycode, int pressed);
    void errorOccured(QString message);

};

#endif // KEYBOARDCONTROLLER_H
