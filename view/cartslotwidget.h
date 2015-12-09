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

#ifndef CARTSLOTWIDGET_H
#define CARTSLOTWIDGET_H

#include <QWidget>

namespace Ui {
    class CartSlotWidget;
}

class CartSlot;

class CartSlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CartSlotWidget(int number, QWidget *parent = 0);
    ~CartSlotWidget();
    void setNewNumber(int number,bool disconnect=true, int displayNumber = -1);
	void showInfo();

    // m2
    void updateSlotCounter(int newValue);

public slots:
    void updatePosition(double pos);
    void updateLength(double length);

private:
    int slotNumber;
    Ui::CartSlotWidget *ui;
    void setColor(bool blink=false);
    double length;
    CartSlot *slot;
    QString normalColor;
    QString blinkColor;
    QString normalFontColor;
    QString blinkFontColor;

protected:
    void mousePressEvent(QMouseEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);

private slots:
    void editCartSlot();
    void stoppedPlaying();
    // m2: Overload to escape stoppedPlaying called after editing slot
    void stoppedPlaying(bool reallyStopped);
    void pauseSlot();
    void startedPlaying();

    // m2:
    void pausedPlaying();
};

#endif // CARTSLOTWIDGET_H
