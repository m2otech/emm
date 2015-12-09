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

#include <math.h>
#include "model/audio/audioprocessor.h"
#include "model/audio/playlistplayer.h"
#include "model/globaldata.h"
#include "model/playlistentry.h"
#include "playerwidget.h"
#include "ui_playerwidget.h"
#include "view/editplayerdialog.h"

PlayerWidget::PlayerWidget(int number, QWidget *parent) :
        QWidget(parent),
        ui(new Ui::PlayerWidget)
{
    ui->setupUi(this);
    ui->numberButton->setText(QString::number(number));
    this->number = number;
    this->player = AudioProcessor::getInstance()->getPlayerWithNumber(number); //PlaylistPlayer::getObjectWithNumber(number);

    showInfo();

    connect(ui->playButton, SIGNAL(clicked()), player, SLOT(play()));
    connect(ui->stopButton, SIGNAL(clicked()), player, SLOT(stop()));
    connect(ui->numberButton, SIGNAL(clicked()), this, SLOT(editPlayer()));
    connect(player,SIGNAL(sendSongLength(double)),this,SLOT(updateLength(double)));
    connect(player,SIGNAL(sendCurrentPosition(double)), this, SLOT(updatePosition(double)));
    connect(player,SIGNAL(entryLoaded(PlaylistEntry*)), this, SLOT(entryLoaded(PlaylistEntry*)));
    connect(player,SIGNAL(stopped(PlaylistEntry*)), this, SLOT(history(PlaylistEntry*)));
}

void PlayerWidget::showInfo()
{
    ui->contentWidget->setStyleSheet("QWidget#contentWidget {border:1px solid #000000; background-color: "+GlobalData::getColorCode(player->getColor())+"}");
    ui->playedLabel->setStyleSheet("font-size:25px; color:"+GlobalData::getColorCode(player->getFontColor()));
    ui->toPlayLabel->setStyleSheet("font-size:25px; color:"+GlobalData::getColorCode(player->getFontColor()));
    ui->artistLabel->setStyleSheet("font-size:20px; font-weight:bold; color:"+GlobalData::getColorCode(player->getFontColor()));
    ui->titleLabel->setStyleSheet("font-size:18px; color:"+GlobalData::getColorCode(player->getFontColor()));
}

PlayerWidget::~PlayerWidget()
{
    disconnect(player,0,0,0);
    this->player->stop();
    delete ui;
}

void PlayerWidget::updateLength(double length)
{
    this->length = length;
    ui->progressBar->setMaximum(length);
}

void PlayerWidget::updatePosition(double pos)
{
    int mins = pos/60;
    int secs = floor(pos-mins*60);
    int msecs = floor((pos-mins*60-secs)*10);
    ui->playedLabel->setText(QString("%1:%2.%3").arg(mins,2, 10, QChar('0')).arg(secs,2,10, QChar('0')).arg(msecs,1,10, QChar('0')));
    double pos2 = (length-pos);
    int mins2 = pos2/60;
    int secs2 = floor(pos2-mins2*60);
    int msecs2 = floor((pos2-mins2*60-secs2)*10);
    ui->toPlayLabel->setText(QString("-%1:%2.%3").arg(mins2,2, 10, QChar('0')).arg(secs2,2,10, QChar('0')).arg(msecs2,1,10, QChar('0')));
    ui->progressBar->setValue(pos);
}

void PlayerWidget::entryLoaded(PlaylistEntry *entry)
{
    if (entry == NULL)
    {
        ui->artistLabel->setText("");
        ui->titleLabel->setText("");
        ui->playedLabel->setText("00:00.0");
        ui->toPlayLabel->setText("-00:00.0");
    }
    else
    {
        ui->artistLabel->setText(entry->getArtist());
        ui->titleLabel->setText(entry->getTitle());
        ui->playedLabel->setText("00:00.0");
        double pos = entry->getLength();
        int mins = pos/60;
        int secs = floor(pos-mins*60);
        int msecs = floor((pos-mins*60-secs)*10);
        ui->toPlayLabel->setText(QString("-%1:%2.%3").arg(mins,2, 10, QChar('0')).arg(secs,2,10, QChar('0')).arg(msecs,1,10, QChar('0')));
    }
    ui->progressBar->setValue(0);
}

int PlayerWidget::getNumber()
{
    return this->number;
}

void PlayerWidget::editPlayer()
{
    EditPlayerDialog *editDialog = new EditPlayerDialog(this->number);
    if (editDialog->exec()==1)
    {
        showInfo();
        player->loadStream();
    }
}


void PlayerWidget::history(PlaylistEntry* entry)
{
    if (entry != NULL)
        emit addToHistory(entry);
}
