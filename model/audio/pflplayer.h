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

#ifndef PFLPLAYER_H
#define PFLPLAYER_H

#include <QTime>
#include "abstractaudioobject.h"

class PFLPlayer : public AbstractAudioObject
{
    Q_OBJECT
public:
    struct VolumeInformation {
        long left;
        long right;
    };

    void analyse(bool readName=false);
    QList<VolumeInformation> getVisualData();
    int getPeak();
    void setFilename(QString filename);
    double getPeakInDB();
    static PFLPlayer* getInstance();
    static void dropInstance();

private:
    PFLPlayer(int, QObject* parent = 0);
    PFLPlayer(const PFLPlayer &);
    PFLPlayer& operator=(const PFLPlayer &);
    static PFLPlayer* instance;

    QList<VolumeInformation> visualInformation;
    int peak;
    double db;

public slots:
    void play();
    void setDB(double db);
    void playCue(QTime,bool end=false);
    void setPosition(double pos);

private slots:
    void readData();
    void saveData(){}

signals:
    void newData(QList<PFLPlayer::VolumeInformation> data);
    void sendName(QString name);

};

#endif // PFLPLAYER_H
