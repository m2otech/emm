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

#ifndef ABSTRACTAUDIOOBJECT_H
#define ABSTRACTAUDIOOBJECT_H

#include <QList>
#include <QMap>
#include <QObject>
#include "bass.h"
#include "bassasio.h"

class QTimer;

class AbstractAudioObject : public QObject
{
    Q_OBJECT
public:
    int getDeviceID();
    int getChannel();
    QString getColor();
    QString getFontColor();
    int getType();
    QString getFileName();
    bool isPlaying();
    bool isPaused();
    void fadeOut(int ms);
    void fadeOutPause(int ms);
    bool isMissing();
    double getTimeLeft();
    // m2: redeclared as slot to call it asynchronously
    //void pause();
    virtual void loadStream(double vol=0);

protected:
    AbstractAudioObject(int , QObject * = 0);

    int type;
    int device;
    int channel;
    int number;
    QString filename;
    bool playing;
    bool fading;
    bool paused;
    HSTREAM stream;
    QString color;
    QString fontColor;
    QTimer *timer;
    QList<BASS_DX8_PARAMEQ> eqs;
	bool fileMissing;

    DWORD getSpeakerFlag();
    void unloadStream();
    void initDevice();
    static double DBToLevel(double db);

signals:
    void sendSongLength(double length);
    void sendCurrentPosition(double pos);
    void stopped();
    void started();

    void pausedSignal();

public slots:
    virtual void play();
    virtual void stop();
    // m2: redeclared as slot to call it asynchronously
    void pause();

protected slots:
    virtual void readData()=0;
    virtual void saveData()=0;
    virtual void updatePosition();

};

#endif // ABSTRACTAUDIOOBJECT_H
