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

#ifndef AUDIOVISUALIZER_H
#define AUDIOVISUALIZER_H

#include <QTime>
#include <QWidget>
#include "model/audio/pflplayer.h"


class AudioVisualizer : public QWidget
{
    Q_OBJECT
public:
    explicit AudioVisualizer(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

private:
    QList<PFLPlayer::VolumeInformation> data;
    double db;
    double length;
    double currentPosition;
    double startpos;
    double endpos;

signals:
    void posChanged(double pixelPos);
    void newStartPos(double startPos);
    void newEndPos(double endPos);
    void setCurrentPosition(double pos);

public slots:
    void updateVolume(double db);
    void zoomIn();
    void zoomOut();
    void setData(QList<PFLPlayer::VolumeInformation> data);
    void setLength(double);
    void setPos(double);
    void setStartPos(QTime);
    void setEndPos(QTime);
    double getCurrentPos();
    double getLength();

};

#endif // AUDIOVISUALIZER_H
