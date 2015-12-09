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

#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QThread>

class CartSlot;
class PlaylistPlayer;
class PFLPlayer;
class MainWindow;

class AudioProcessor : public QThread
{
    Q_OBJECT
public:
    void initDevices(MainWindow* mw);
    void freeDevices();
    CartSlot* getCartSlotWithNumber(int number);
    PlaylistPlayer* getPlayerWithNumber(int number);
    PFLPlayer* getPFLPlayer();
    static AudioProcessor* getInstance();
    static void dropInstance();

private:
    AudioProcessor(QObject* parent = 0){}
    AudioProcessor(const AudioProcessor &);
    AudioProcessor& operator=(const AudioProcessor &);
    static AudioProcessor* instance;

protected:
    void run();

};

#endif // AUDIOPROCESSOR_H
