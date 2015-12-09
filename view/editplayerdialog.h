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

#ifndef EDITPLAYERDIALOG_H
#define EDITPLAYERDIALOG_H

#include <QDialog>

namespace Ui {
    class EditPlayerDialog;
}

class EditPlayerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPlayerDialog(int playerNumber, QWidget *parent = 0);
    ~EditPlayerDialog();

private:
    int playerNumber;
    Ui::EditPlayerDialog *ui;
    QString color;
    QString fontColor;
    void setColor(QString color);
    void setFontColor(QString color);

public slots:
    void closeDialog();
    void save();

private slots:
    void selectColor();
    void selectFontColor();

};

#endif // EDITPLAYERDIALOG_H
