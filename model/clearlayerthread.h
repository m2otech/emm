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

#ifndef CLEARLAYERTHREAD_H
#define CLEARLAYERTHREAD_H

#include <QThread>

class ClearLayerThread : public QThread
{
    Q_OBJECT
public:
    explicit ClearLayerThread(int selectedLayer, QObject *parent = 0);

protected:
    void run();

private:
    int selectedLayer;

signals:
    void updateMax(int);
    void updateStatus(int);
};

#endif // CLEARLAYERTHREAD_H
