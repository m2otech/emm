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

#ifndef EDITCARTSLOTDIALOG_H
#define EDITCARTSLOTDIALOG_H

#include <QDialog>

namespace Ui {
    class EditCartSlotDialog;
}

class AudioVisualizer;
class PFLPlayer;
class CartSlot;

class EditCartSlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditCartSlotDialog(int slotNumber, bool db=false, QWidget *parent = 0);
    ~EditCartSlotDialog();

private:
    int slotNumber;
    Ui::EditCartSlotDialog *ui;
    AudioVisualizer* visualWidget;
    PFLPlayer* player;
    QString color;
    QString fontColor;
    void setColor(QString color);
    void setFontColor(QString color);
    bool database;
    void setData();
    void loadData(CartSlot* slot, bool db);

private slots:
    void closeDialog();
    void save();
    void getMaxVol();
    void updateScrollPosition(double pixelPos);
    void setNewStartPos(double);
    void setNewEndPos(double);
    void playCue();
    void stopCue();
    void selectFile();
    void selectColor();
    void selectFontColor();
    void loadFromDB();
    void saveToDB();
    void setCurrentPosAsStartPos();
    void setCurrentPosAsStopPos();
};

#endif // EDITCARTSLOTDIALOG_H
