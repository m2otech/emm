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
#include <QTimer>
#include <QMessageBox>
#include "keyboardcontroller.h"

KeyboardController* KeyboardController::instance = 0;
int KeyboardController::keyCommandCount = 0;
int KeyboardController::playerCount = 0;

int KeyboardController::initializeKeyboardController()
{
    hMWXUSB = LoadLibrary(L"MWXUSB.DLL");
    if (hMWXUSB == NULL) {
        // m2: Warning wird in main.cpp ausgegeben
        //emit errorOccured(tr("Der Tastaturtreiber ist nicht korrekt installiert. Die Steuerung durch die externe Tastatur wird deaktiviert."));
        return 1;
    }
    lpopenusb = (OPEN_USB*)GetProcAddress(hMWXUSB,"Open_USB");
    lpcloseusb=(CLOSE_USB*)GetProcAddress(hMWXUSB,"Close_USB");
    lpcallback=(SET_CALLBACK*)GetProcAddress(hMWXUSB,"Set_Callback");
    lpposkey=(READ_POS_KEY_USB*)GetProcAddress(hMWXUSB,"read_POS_key_USB");
    lpled=(ACCEPT_LED*)GetProcAddress(hMWXUSB,"Accept_LED");

    if (lpopenusb == NULL || lpcloseusb == NULL || lpcallback == NULL || lpposkey == NULL || lpled == NULL) {
        //emit errorOccured(tr("Fehler bei der Initialisierung der Verbindung zur Tastatur. Die Steuerung durch die externe Tastatur wird deaktiviert."));
        return 2;
    }

    unsigned char ucOpen = (*lpopenusb)();
    if(ucOpen != 0)
    {
        //emit errorOccured(tr("Es konnte keine Verbindung zur Controller-Tastatur hergestellt werden. Bitte vergewissern Sie sich, dass die Tastatur angeschlossen ist. Die Steuerung durch die externe Tastatur wird deaktiviert."));
        closeKeyboardController();
        return 3;
    }
    opened = true;
    (*lpcallback)( &KeyboardController::keyboardCallback );

    return 0;
}

void KeyboardController::closeKeyboardController()
{
    if (lpcloseusb != NULL && opened) {
        (*lpcloseusb)();
    }
    FreeLibrary( hMWXUSB );
    hMWXUSB = NULL;
}

void KeyboardController::sendKeyCommand()
{
    unsigned char ch;
    int ibuf[2];
    ch = (*lpposkey)(ibuf,10);
    emit keyPressed(ibuf[0],ibuf[1]);
}

void KeyboardController::increasePlayerCount()
{
    playerCount++;
}

void KeyboardController::reducePlayerCount()
{
    playerCount--;
}

void __stdcall KeyboardController::keyboardCallback(int value)
{
    if(value == 1 || value == 2)
    {
        KeyboardController::getInstance()->sendKeyCommand();
    }
}

KeyboardController* KeyboardController::getInstance()
{
    static QMutex mutex;
    if (!instance)
    {
        mutex.lock();

        if (!instance) {
            instance = new KeyboardController;
            instance->hMWXUSB = NULL;
            instance->opened = false;
        }

        mutex.unlock();
    }
    return instance;
}

void KeyboardController::dropInstance()
{
    static QMutex mutex;
    mutex.lock();
    delete instance;
    instance = 0;
    mutex.unlock();
}
