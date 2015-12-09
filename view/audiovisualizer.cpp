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

#include <QPaintEvent>
#include <QPainter>
#include <math.h>
#include "audiovisualizer.h"

AudioVisualizer::AudioVisualizer(QWidget *parent) :
        QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->db = 0;
    this->currentPosition = 0;
    this->length = 0;
}

void AudioVisualizer::setData(QList<PFLPlayer::VolumeInformation> data)
{
    this->data = data;
    this->repaint();
}

void AudioVisualizer::updateVolume(double db)
{
    this->db = db;
    this->repaint();
}

void AudioVisualizer::zoomIn()
{
    this->setFixedWidth(this->width()+100);
    this->repaint();
}

void AudioVisualizer::zoomOut()
{
    this->setFixedWidth(this->width()-100);
    this->repaint();
}

void AudioVisualizer::setLength(double length)
{
    this->length = length;
}

void AudioVisualizer::setPos(double pos)
{
    this->currentPosition = pos;
    this->repaint();
}

void AudioVisualizer::mousePressEvent(QMouseEvent *e)
{
    double pixelPos = e->localPos().x();
    double pos = pixelPos/(double)this->width()*length;
    if (e->button() == Qt::LeftButton && e->type() == QEvent::MouseButtonDblClick)
    {
        emit newStartPos(pos);
        startpos = pos;
    }
    else if (e->button() == Qt::RightButton && e->type() == QEvent::MouseButtonDblClick)
    {
        emit newEndPos(length-pos);
        endpos = pos;
    }
    else if (e->button() == Qt::LeftButton)
    {
        emit setCurrentPosition(pos);
    }
    this->repaint();
}

void AudioVisualizer::wheelEvent(QWheelEvent *e)
{
    int numDegrees = e->delta() / 8;
    int numSteps = numDegrees / 15;
    int newWidth = this->width()+numSteps*100;
    if (newWidth >= this->parentWidget()->width())
        this->setFixedWidth(this->width()+numSteps*100);
}

void AudioVisualizer::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QPen pen = painter.pen();

    painter.fillRect(0,0,this->width(),this->height(),QBrush(Qt::black));

    double penWidth = this->width()/(double)data.size();
    pen.setWidthF(penWidth);
    pen.setColor(Qt::cyan);
    painter.setPen(pen);

    double pos=0;
    double maxVol=0;
    double lineY = this->height()/2;
    double level = 32768*pow(10,db/20);
    for (int i=0;i<data.size();i++)
    {
        double left = (double)data.at(i).left*level/32768;
        left = left*(this->height()/2)/32768;
        double right = (double)data.at(i).right*level/32768;
        right = right*(this->height()/2)/32768;
        maxVol = qMax(maxVol,(double)data.at(i).left);
        maxVol = qMax(maxVol,(double)data.at(i).right);

        painter.drawLine(QPointF(pos,lineY),QPointF(pos,lineY-left));
        painter.drawLine(QPointF(pos,lineY),QPointF(pos,lineY+right));
        pos += penWidth;
    }

    pen.setWidthF(1.0);
    pen.setColor(Qt::magenta);
    painter.setPen(pen);

    painter.drawLine(0,lineY,this->width(),lineY);

    pen.setColor(Qt::white);
    painter.setPen(pen);

    double lineX = this->currentPosition*(data.size()/length)*penWidth;

    painter.drawLine(lineX,0,lineX,this->height());
    emit posChanged(lineX);

    pen.setColor(Qt::green);
    painter.setPen(pen);

    QFont font(painter.font());
    font.setBold(true);
    painter.setFont(font);
    QFontMetricsF fm(painter.font());

    if (startpos != 0) {
        lineX = startpos*(data.size()/length)*penWidth;
        painter.drawLine(lineX,0,lineX,this->height());
        QString sp = "START";
        painter.drawText(QRectF(lineX+5,5,fm.width(sp),fm.height()),sp);
    }

    if (endpos != length) {
        lineX = endpos*(data.size()/length)*penWidth;
        painter.drawLine(lineX,0,lineX,this->height());
        QString st = "STOP";
        painter.drawText(QRectF(lineX-5-fm.width(st),5,fm.width(st),fm.height()),st);
    }

}

void AudioVisualizer::setStartPos(QTime startTime) {
    startpos = startTime.minute()*60+startTime.second()+(double)startTime.msec()/1000;
    this->repaint();
}

void AudioVisualizer::setEndPos(QTime stopTime) {
    endpos = length-(stopTime.minute()*60+stopTime.second()+(double)stopTime.msec()/1000);
    this->repaint();
}

double AudioVisualizer::getCurrentPos() {
    return this->currentPosition;
}

double AudioVisualizer::getLength() {
    return this->length;
}
