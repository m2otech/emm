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

#ifndef LOADSLOTDIALOG_H
#define LOADSLOTDIALOG_H

#include <QDialog>

namespace Ui {
    class LoadSlotDialog;
}

class LoadSlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadSlotDialog(QWidget *parent = 0);
    ~LoadSlotDialog();

private:
    Ui::LoadSlotDialog *ui;

private slots:
    void selectEntry();
};

#endif // LOADSLOTDIALOG_H
