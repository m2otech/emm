/* ***************************************************************************
 * This file is part of Event Music Machine.
 *
 * Event Music Machine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Event Music Machine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Event Music Machine. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************* */

#include <QDebug>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QScrollBar>
#include <math.h>
#include "editcartslotdialog.h"
#include "loadslotdialog.h"
#include "model/audio/audioprocessor.h"
#include "model/audio/bassasiodevice.h"
#include "model/audio/bassdevice.h"
#include "model/audio/cartslot.h"
#include "model/audio/pflplayer.h"
#include "model/globaldata.h"
#include "ui_editcartslotdialog.h"
#include "view/audiovisualizer.h"

// m2:
#include "model/configuration.h"
#include "view/mainwindow.h"

EditCartSlotDialog::EditCartSlotDialog(int slotNumber,bool db, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::EditCartSlotDialog)
{
    ui->setupUi(this);
    this->slotNumber = slotNumber;
    ui->scrollArea->horizontalScrollBar()->setMaximum(10000);
    player = AudioProcessor::getInstance()->getPFLPlayer();

    CartSlot *slot = CartSlot::getObjectWithNumber(slotNumber,db);
    this->database = db;
    loadData(slot,db);

    visualWidget = new AudioVisualizer(ui->scrollArea);
    ui->scrollArea->setWidget(visualWidget);
    connect(player, SIGNAL(newData(QList<PFLPlayer::VolumeInformation>)), visualWidget, SLOT(setData(QList<PFLPlayer::VolumeInformation>)));
    connect(player, SIGNAL(sendSongLength(double)), visualWidget, SLOT(setLength(double)));
    connect(player, SIGNAL(sendCurrentPosition(double)), visualWidget, SLOT(setPos(double)));
    connect(player, SIGNAL(sendName(QString)), ui->line1Text, SLOT(setText(QString)));
    connect(visualWidget, SIGNAL(setCurrentPosition(double)), player, SLOT(setPosition(double)));

    player->setFilename(ui->fileText->text());
    player->analyse();
    player->setDB(ui->volSpinBox->value());
    visualWidget->updateVolume(ui->volSpinBox->value());
    player->play();
    player->stop();

    visualWidget->setStartPos(ui->startPosTime->time());
    visualWidget->setEndPos(ui->stopPosTime->time());

    if (db)
    {
        ui->deviceSelectWidget->setVisible(false);
        ui->saveSlotButton->setVisible(false);
        ui->loadSlotButton->setVisible(false);
    }

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(save()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(closeDialog()));
    connect(ui->selectFileButton, SIGNAL(clicked()), this, SLOT(selectFile()));
    connect(ui->getMaxVolButton, SIGNAL(clicked()), this, SLOT(getMaxVol()));
    connect(ui->volSpinBox, SIGNAL(valueChanged(double)), visualWidget, SLOT(updateVolume(double)));
    connect(ui->zoomInButton, SIGNAL(clicked()), visualWidget, SLOT(zoomIn()));
    connect(ui->zoomOutButton, SIGNAL(clicked()), visualWidget, SLOT(zoomOut()));
    connect(ui->playButton, SIGNAL(clicked()), player, SLOT(play()));
    connect(ui->stopButton, SIGNAL(clicked()), player, SLOT(stop()));
    connect(ui->volSpinBox, SIGNAL(valueChanged(double)), player, SLOT(setDB(double)));
    connect(visualWidget, SIGNAL(posChanged(double)), this, SLOT(updateScrollPosition(double)));
    connect(visualWidget, SIGNAL(newStartPos(double)), this, SLOT(setNewStartPos(double)));
    connect(visualWidget, SIGNAL(newEndPos(double)), this, SLOT(setNewEndPos(double)));
    connect(ui->startPosTime, SIGNAL(timeChanged(QTime)), visualWidget, SLOT(setStartPos(QTime)));
    connect(ui->stopPosTime, SIGNAL(timeChanged(QTime)), visualWidget, SLOT(setEndPos(QTime)));
    connect(ui->playCueButton, SIGNAL(clicked()), this, SLOT(playCue()));
    connect(ui->stopCueButton, SIGNAL(clicked()), this, SLOT(stopCue()));
    connect(ui->selectColorButton, SIGNAL(clicked()), this, SLOT(selectColor()));
    connect(ui->selectFontColorButton, SIGNAL(clicked()), this, SLOT(selectFontColor()));
    connect(ui->saveSlotButton, SIGNAL(clicked()), this, SLOT(saveToDB()));
    connect(ui->loadSlotButton, SIGNAL(clicked()), this, SLOT(loadFromDB()));
    connect(ui->setStartPointButton, SIGNAL(clicked()), this, SLOT(setCurrentPosAsStartPos()));
    connect(ui->setStopPointButton, SIGNAL(clicked()), this, SLOT(setCurrentPosAsStopPos()));
    connect(ui->swapColorsButton, SIGNAL(clicked()), this, SLOT(swapColors()));

    // m2: On opening dialog place cursor in Title field
    ui->line1Text->setFocus();
    ui->line1Text->setCursorPosition(ui->line1Text->text().length());
}

EditCartSlotDialog::~EditCartSlotDialog()
{
    delete ui;
}

void EditCartSlotDialog::save()
{
    player->stop();
    setData();
    done(1);
    MainWindow::getInstance()->updateSlotAssignment();
}

void EditCartSlotDialog::loadData(CartSlot *slot, bool db)
{
    ui->fileText->setText(slot->getFileName());
    ui->line1Text->setText(slot->getText1());
    if (!db)
        ui->deviceSelectWidget->selectDevice(slot->getType(),slot->getDeviceID(),slot->getChannel());
    setColor(slot->getColor());
    setFontColor(slot->getFontColor());

    // m2: set SF to black if slot empty
    if (slot->getFileName().isEmpty() && slot->getText1().isEmpty())
        setFontColor("#000000");

    ui->letFadeCheckBox->setChecked(slot->getLetFade());
    ui->fadeOutCheckBox->setChecked(slot->getFadeOut());
    ui->disablePause->setChecked(slot->getPauseDisabled());
    ui->fadeOthersCheckBox->setChecked(slot->getFadeOthers());
    ui->loopCheckBox->setChecked(slot->getLoop());
    ui->enableCup->setChecked(slot->getCupEnabled());
    int minStart = floor(slot->getStartPos()/60);
    int secsStart = floor(slot->getStartPos()-minStart*60);
    int msecsStart = (slot->getStartPos()-secsStart-minStart*60)*1000;
    ui->startPosTime->setTime(QTime(0,minStart,secsStart,msecsStart));
    int minStop = floor(slot->getStopPos()/60);
    int secsStop = floor(slot->getStopPos())-minStop*60;
    int msecsStop = (slot->getStopPos()-secsStop-minStop*60)*1000;
    ui->stopPosTime->setTime(QTime(0,minStop,secsStop,msecsStop));
    ui->pitchSpinBox->setValue(slot->getPitch());
    ui->fontSizeSpinBox->setValue(slot->getFontSize());
    ui->volSpinBox->setValue(slot->getDB());
    ui->eqActiveCheckBox->setChecked(slot->getEQActive());
    QString config = slot->getEQConfig();
    QList<QString> parts = config.split(";");
    for(int i=0;i<parts.length();i++) {
        switch (i) {
            case 0: ui->eqCh1->setValue(parts.at(i).toInt()); break;
            case 1: ui->eqCh2->setValue(parts.at(i).toInt()); break;
            case 2: ui->eqCh3->setValue(parts.at(i).toInt()); break;
            case 3: ui->eqCh4->setValue(parts.at(i).toInt()); break;
            case 4: ui->eqCh5->setValue(parts.at(i).toInt()); break;
            case 5: ui->eqCh6->setValue(parts.at(i).toInt()); break;
            case 6: ui->eqCh7->setValue(parts.at(i).toInt()); break;
            case 7: ui->eqCh8->setValue(parts.at(i).toInt()); break;
            case 8: ui->eqCh9->setValue(parts.at(i).toInt()); break;
            case 9: ui->eqCh10->setValue(parts.at(i).toInt()); break;
        }
    }
    setColor(slot->getColor());
}

void EditCartSlotDialog::setData()
{
    CartSlot *slot = CartSlot::getObjectWithNumber(slotNumber,database);
    QTime startTime = ui->startPosTime->time();
    QTime stopTime = ui->stopPosTime->time();
    double startPos = startTime.minute()*60+startTime.second()+(double)startTime.msec()/1000;
    double stopPos = stopTime.minute()*60+stopTime.second()+(double)stopTime.msec()/1000;
    slot->setDataAndSave(
        ui->fileText->text(),
        ui->line1Text->text(),
        ui->deviceSelectWidget->getDriver(),
        ui->deviceSelectWidget->getDevice(),
        ui->deviceSelectWidget->getChannel(),
        color,
        fontColor,
        ui->fadeOutCheckBox->isChecked(),
        ui->letFadeCheckBox->isChecked(),
        ui->fadeOthersCheckBox->isChecked(),
        ui->loopCheckBox->isChecked(),
        startPos,
        stopPos,
        ui->pitchSpinBox->value(),
        ui->fontSizeSpinBox->value(),
        ui->volSpinBox->value(),
        ui->eqActiveCheckBox->isChecked(),
        QString::number(ui->eqCh1->value())+';'+QString::number(ui->eqCh2->value())+';'+QString::number(ui->eqCh3->value())+';'+QString::number(ui->eqCh4->value())+';'+QString::number(ui->eqCh5->value())+';'+QString::number(ui->eqCh6->value())+';'+QString::number(ui->eqCh7->value())+';'+QString::number(ui->eqCh8->value())+';'+QString::number(ui->eqCh9->value())+';'+QString::number(ui->eqCh10->value()),
        // m2: new checkboxes disable pause / enable CUP
        ui->disablePause->isChecked(),
        ui->enableCup->isChecked()
    );
}

void EditCartSlotDialog::closeDialog()
{
    player->stop();
    done(0);
}

void EditCartSlotDialog::selectFile()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setNameFilters(GlobalData::getSupportedAudioFormats());
    dialog.setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint);
    if (dialog.exec())
        if (dialog.selectedFiles().size()>0) {
            if (CartSlot::isUsed(dialog.selectedFiles().first()) != -1) {
                int res = QMessageBox::question(this, tr("Titel wird bereits verwendet"), tr("Dieser Titel ist schon in einem anderen Slot geladen. Soll er trotzdem ausgewählt werden?"), QMessageBox::Yes, QMessageBox::No);
                if (res == QMessageBox::No) {
                    return;
                }
            }
            ui->fileText->setText(dialog.selectedFiles().first());
            player->setFilename(ui->fileText->text());
            player->analyse(true);
        }
}

void EditCartSlotDialog::setColor(QString color)
{
    QString colorCode=GlobalData::getColorCode(color);
    ui->colorLabel->setStyleSheet("border:1px solid #000000; background-color: "+colorCode);
    this->color = colorCode;
    //qDebug("SET Color code: " + colorCode.toUtf8());
}

void EditCartSlotDialog::setFontColor(QString color)
{
    QString colorCode=GlobalData::getColorCode(color);
    ui->fontColorLabel->setStyleSheet("border:1px solid #000000; background-color: "+colorCode);
    this->fontColor = colorCode;
}

void EditCartSlotDialog::getMaxVol()
{
    ui->volSpinBox->setValue(-player->getPeakInDB());
}

void EditCartSlotDialog::updateScrollPosition(double pixelPos)
{
    if (!player->isPlaying() || !ui->autoscrollButton->isChecked())
        return;
    double saWidth = ui->scrollArea->width();
    double vaWidth = visualWidget->width();
    int max = ui->scrollArea->horizontalScrollBar()->maximum();
    int newPos;

    if (vaWidth <= saWidth)
        return;

    if (pixelPos < (saWidth/2))
        newPos = 0;

    if ((vaWidth-pixelPos) < (saWidth/2))
        newPos = max;


    double middle = vaWidth-saWidth;
    newPos = pixelPos-(saWidth/2);

    ui->scrollArea->horizontalScrollBar()->setValue(max*newPos/middle);
}

void EditCartSlotDialog::setCurrentPosAsStartPos() {
    setNewStartPos(visualWidget->getCurrentPos());
}

void EditCartSlotDialog::setCurrentPosAsStopPos() {
    setNewEndPos(visualWidget->getLength()-visualWidget->getCurrentPos());
}

void EditCartSlotDialog::setNewStartPos(double pos)
{
    int minStart = floor(pos/60);
    int secsStart = floor(pos-minStart*60);
    int msecsStart = (pos-secsStart-minStart*60)*1000;
    ui->startPosTime->setTime(QTime(0,minStart,secsStart,msecsStart));
}

void EditCartSlotDialog::setNewEndPos(double pos) {
    int minStop = floor(pos/60);
    int secsStop = floor(pos)-minStop*60;
    int msecsStop = (pos-secsStop-minStop*60)*1000;
    ui->stopPosTime->setTime(QTime(0,minStop,secsStop,msecsStop));
}

void EditCartSlotDialog::playCue() {
    player->playCue(ui->startPosTime->time());
}

void EditCartSlotDialog::stopCue() {
    player->playCue(ui->stopPosTime->time(),true);
}

void EditCartSlotDialog::selectColor()
{
    QColor selectedColor;
    selectedColor = QColorDialog::getColor(QColor(color),this);
    if (selectedColor.isValid())
    {
        setColor(selectedColor.name());
    }
}

void EditCartSlotDialog::selectFontColor()
{
    QColor selectedColor;
    selectedColor = QColorDialog::getColor(QColor(fontColor),this);
    if (selectedColor.isValid())
    {
        setFontColor(selectedColor.name());
    }
}

void EditCartSlotDialog::saveToDB()
{
    setData();
    CartSlot *slot = CartSlot::getObjectWithNumber(slotNumber,database);
    slot->saveInDatabase();
    QMessageBox::information(this,tr("Slot gespeichert"),tr("Slot wurde im Slot-Speicher abgelegt."));
}

void EditCartSlotDialog::loadFromDB()
{
    //if (!QFile("slotstore.emm").exists())
    // m2: providing full path for slotstore.emm
    if (!QFile(Configuration::getStorageLocation() + "/slotstore.emm").exists())
    {
        QMessageBox::information(this,tr("Slot-Speicher existiert nicht."),tr("Es wurde keine gültige Slot-Datenbank gefunden."));
    }
    else
    {
        LoadSlotDialog *lsd = new LoadSlotDialog(this);
        int id = lsd->exec();
        if (id>0)
        {
            CartSlot *slot = CartSlot::getObjectWithNumber(id,true);
            loadData(slot,true);
        }
    }
}

void EditCartSlotDialog::swapColors()
{
    //QString ftSS = ui->fontColorLabel->styleSheet();
    //QString bgSS = ui->colorLabel->styleSheet();
    QString ftColor = this->fontColor;
    QString bgColor = this->color;

    setColor(ftColor);
    setFontColor(bgColor);
    //ui->fontColorLabel->setStyleSheet(bgSS);
    //ui->colorLabel->setStyleSheet(ftSS);
}
