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

#include <QColorDialog>
#include <math.h>
#include "editplayerdialog.h"
#include "model/audio/playlistplayer.h"
#include "model/globaldata.h"
#include "ui_editplayerdialog.h"

EditPlayerDialog::EditPlayerDialog(int playerNumber,QWidget *parent) :
        QDialog(parent),
        ui(new Ui::EditPlayerDialog)
{
    ui->setupUi(this);
    this->playerNumber = playerNumber;

    PlaylistPlayer *player = PlaylistPlayer::getObjectWithNumber(playerNumber);
    ui->deviceSelectWidget->selectDevice(player->getType(),player->getDeviceID(),player->getChannel());
    setColor(player->getColor());
    setFontColor(player->getFontColor());

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(save()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(closeDialog()));
    connect(ui->selectColorButton, SIGNAL(clicked()), this, SLOT(selectColor()));
    connect(ui->selectFontColorButton, SIGNAL(clicked()), this, SLOT(selectFontColor()));
}

EditPlayerDialog::~EditPlayerDialog()
{
    delete ui;
}

void EditPlayerDialog::save()
{
    PlaylistPlayer *player = PlaylistPlayer::getObjectWithNumber(playerNumber);
    player->setDataAndSave(
            ui->deviceSelectWidget->getDriver(),
            ui->deviceSelectWidget->getDevice(),
            ui->deviceSelectWidget->getChannel(),
            color,
            fontColor
            );
    done(1);
}

void EditPlayerDialog::closeDialog()
{
    done(0);
}

void EditPlayerDialog::setColor(QString color)
{
    QString colorCode=GlobalData::getColorCode(color);
    ui->colorLabel->setStyleSheet("border:1px solid #000000; background-color: "+colorCode);
    this->color = colorCode;
}

void EditPlayerDialog::setFontColor(QString color)
{
    QString colorCode=GlobalData::getColorCode(color);
    ui->fontColorLabel->setStyleSheet("border:1px solid #000000; background-color: "+colorCode);
    this->fontColor = colorCode;
}

void EditPlayerDialog::selectColor()
{
    QColor selectedColor;
    selectedColor = QColorDialog::getColor(QColor(color),this);
    if (selectedColor.isValid())
    {
        setColor(selectedColor.name());
    }
}

void EditPlayerDialog::selectFontColor()
{
    QColor selectedColor;
    selectedColor = QColorDialog::getColor(QColor(fontColor),this);
    if (selectedColor.isValid())
    {
        setFontColor(selectedColor.name());
    }
}
