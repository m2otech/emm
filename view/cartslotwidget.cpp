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

#include <QDrag>
#include <QDropEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <math.h>
#include "cartslotwidget.h"
#include "editcartslotdialog.h"
#include "model/audio/audioprocessor.h"
#include "model/audio/cartslot.h"
#include "model/configuration.h"
#include "model/globaldata.h"
#include "ui_cartslotwidget.h"

#include "mainwindow.h"

CartSlotWidget::CartSlotWidget(int number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CartSlotWidget)
{
    ui->setupUi(this);
    ui->contentWidget->setBackgroundRole(QPalette::Background);
    this->setNewNumber(number,false);
    this->setAcceptDrops(true);
    ui->pauseButton->setVisible(false);
    //ui->warningIcon->setVisible(false);
}

CartSlotWidget::~CartSlotWidget()
{
    delete ui;
}

void CartSlotWidget::setNewNumber(int number, bool dis, int displayNumber)
{
    if (displayNumber == -1) {
        displayNumber = number;
    }
    ui->editButton->setText(QString::number(displayNumber));
    this->slotNumber = number;
    if (dis) {
        disconnect(slot,0,0,0);
        disconnect(ui->pauseButton, 0, 0, 0);
    }
    this->slot = AudioProcessor::getInstance()->getCartSlotWithNumber(number);

    // m2: Add warning sign if song file not found
    if (slot->isMissing())
        ui->warningIcon->setVisible(true);
        //ui->text1Label->setStyleSheet("background:transparent;\ntext-align:center;\nborder-image: url(:/icons/warning.png)");
    else
        ui->warningIcon->setVisible(false);
        //ui->text1Label->setStyleSheet("background:transparent;\ntext-align:center;");

    showInfo();
    // m2: removed progress bar
    //ui->progressBar->setValue(100);

    //ui->pauseButton->setVisible(slot->isPlaying() || slot->isPaused());
    // m2: also consider new checkbox disable pause and "disable pause" checkbox in Settings
    ui->pauseButton->setVisible( (slot->isPlaying() || slot->isPaused()) && !slot->getPauseDisabled() && Configuration::getInstance()->getPauseButton());
   // Configuration::getInstance()->getPauseButton()
    connect(slot, SIGNAL(sendSongLength(double)), this, SLOT(updateLength(double)));
    connect(slot, SIGNAL(sendCurrentPosition(double)), this, SLOT(updatePosition(double)));
    connect(slot, SIGNAL(stopped()), this, SLOT(stoppedPlaying()));
    connect(slot, SIGNAL(startedPlaying()), this, SLOT(startedPlaying()));
    // m2:
    connect(slot, SIGNAL(pausedSignal()), this, SLOT(pausedPlaying()));

    connect(ui->pauseButton, SIGNAL(clicked()), this, SLOT(pauseSlot()));

    updateLength(slot->getLength());

    // Update slot counter (useful if reset to zero)
    updateSlotCounter(MainWindow::getInstance()->playedCtr[(slot->getNumber() - 1)]);
}

void CartSlotWidget::showInfo()
{
    this->normalColor = GlobalData::getColorCode(slot->getColor());
    QColor col = QColor(this->normalColor);
    int r,g,b;
    col.getRgb(&r,&g,&b);
    QColor col2;
    col2.setRgb(255-r,255-g,255-b);
    this->blinkColor = col2.name();

    this->normalFontColor = slot->getFontColor();
    QColor col3 = QColor(this->normalFontColor);
    col3.getRgb(&r,&g,&b);
    QColor col4;
    col4.setRgb(255-r,255-g,255-b);
    this->blinkFontColor = col4.name();

    ui->text1Label->setText("<center>"+slot->getText1()+"</center>");

    QFont font = ui->text1Label->font();
    font.setPixelSize(slot->getFontSize());
    font.setBold(true);
    ui->text1Label->setFont(font);

    QPalette p(ui->text1Label->palette());
    p.setColor(QPalette::Text,QColor(this->normalFontColor));

    ui->text1Label->setPalette(p);
    ui->toPlayLabel->setPalette(p);
    QFont font2 = ui->toPlayLabel->font();
    font2.setPixelSize(Configuration::getInstance()->getSlotTimeSize());
    ui->toPlayLabel->setFont(font2);

    QString time = slot->getTimeToPlay();
    ui->toPlayLabel->setText(time);

    this->setColor();
    slot->loadEqualizer();
}

void CartSlotWidget::setColor(bool blink)
{
    QString color,fontColor;
    if (blink) {
        color = this->blinkColor;
        fontColor = this->blinkFontColor;
    } else {
        color = this->normalColor;
        fontColor = this->normalFontColor;
    }
    QPalette p(ui->contentWidget->palette());
    p.setColor(QPalette::Background, QColor(color));
    ui->contentWidget->setPalette(p);

    QPalette t(ui->text1Label->palette());
    t.setColor(QPalette::Text, QColor(fontColor));
    t.setColor(QPalette::Foreground, QColor(fontColor));
    ui->text1Label->setPalette(t);
    ui->toPlayLabel->setPalette(t);

    // m2: set colors of slot counter
    ui->playedCounter->setStyleSheet("QLabel { background-color : " + color + "; color : " + fontColor + "; \
                                        font: bold 11px; \
                                        border-style: solid; \
                                        border-width: 1px; \
                                        border-radius: 4px; \
                                        border-color: " + fontColor + "; \
                                        padding-bottom: 1px; \
                                    }");

    // m2: also styling slot number to match (with colors fixed)
    ui->editButton->setStyleSheet("QLabel { background-color : white; color : black; \
                                    font: bold 11px; \
                                    border-style: solid; \
                                    border-width: 1px; \
                                    border-radius: 4px; \
                                    border-color: black; \
                                    padding-bottom: 1px; \
                                }");
}

void CartSlotWidget::editCartSlot()
{
    EditCartSlotDialog *editDialog = new EditCartSlotDialog(this->slotNumber);
    if (editDialog->exec()==1) {
        slot->fetchLength();
        showInfo();

        // m2: set reallyStopped to false to avoid incrementing counter
        // this->stoppedPlaying();
        this->stoppedPlaying(false);
    }
}

void CartSlotWidget::updateLength(double length)
{
    this->length = length;
    if (length > 0) {
        //ui->progressBar->setMaximum(length);
        //ui->progressBar->setValue(length);
    }
}

void CartSlotWidget::updatePosition(double pos)
{
    double pos2 = (length-pos);
    int mins2 = pos2/60;
    int secs2 = floor(pos2-mins2*60);
    int msecs2 = floor((pos2-mins2*60-secs2)*10);
    //ui->progressBar->setValue(length-pos);
    QString time = QString("%1:%2.%3").arg(mins2,2, 10, QChar('0')).arg(secs2,2,10, QChar('0')).arg(msecs2);
    ui->toPlayLabel->setText(time);

    if (secs2%2==0)
    {
        this->setColor(true);
    }
    else
    {
        this->setColor(false);
    }
}

void CartSlotWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if (slot->getCupEnabled()) {
            slot->stop();
            //slot->play();
        }
    }
}

void CartSlotWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->modifiers() == Qt::ShiftModifier) {
        slot->pause();
    } else if (e->modifiers() == Qt::ControlModifier) {
        QDrag *dr = new QDrag(this);
        QMimeData *mimeData = new QMimeData();
        int id = slot->getNumber();
        QByteArray encodedData = QByteArray::number(id);
        mimeData->setData("application/emm.store.slotid",encodedData);
        dr->setMimeData(mimeData);
        dr->start();
    } else if (e->modifiers() == Qt::AltModifier) {
        int res = QMessageBox::question(this,tr("Wirklich löschen?"),tr("Soll der Slot wirklich gelöscht werden?"),QMessageBox::Yes,QMessageBox::No);
        if (res == QMessageBox::Yes) {
            slot->empty();
            showInfo();
        }
    } else {
        if (e->button() == Qt::LeftButton) {
            // m2: Disable jumping back to previous layer if starting song with mouse
            MainWindow::getInstance()->prevLayer = -1;
            slot->play();
        }
        else
            editCartSlot();
    }
}

void CartSlotWidget::stoppedPlaying()
{
    stoppedPlaying(true);
}

void CartSlotWidget::stoppedPlaying(bool reallyStopped)
{
    updateLength(slot->getLength());
    //updatePosition(0);
    updatePosition(slot->getStartPos());
    //ui->progressBar->setValue(ui->progressBar->maximum());
    this->setColor();
    ui->pauseButton->setVisible(false);

    if (reallyStopped) {
        // m2: increase slot counter
        MainWindow::getInstance()->increaseCounter(slot->getNumber() - 1);
        updateSlotCounter(MainWindow::getInstance()->playedCtr[(slot->getNumber() - 1)]);

        // m2: change to previously selected layer (if keyboard controls layer 1 only)
        // v2.3: disabled because now we don't change to layer 1 on hotkey press
        //if ( !(Configuration::getInstance()->getLayerKeyboardSync()) )
        //        MainWindow::getInstance()->resetLayer();

        // m2: set position/layer to 0 and remove song from queue
        //     also method stop in AbstractAudioObject does the same,
        //     because this here is not called when slot layer is not shown
        MainWindow::getInstance()->updateCurrSongPosition(0, 0);
        MainWindow::getInstance()->infoBoxRemoveFromQueue(this->slotNumber);
    }
}

// m2: this will be called when emitting signal pausedSignal
void CartSlotWidget::pausedPlaying()
{
    // m2: set color to "normal" when paused
    this->setColor();
}

void CartSlotWidget::startedPlaying()
{
    //if (Configuration::getInstance()->getPauseButton()) {
    // m2: also check if pause is enabled on this slot
    if ( (Configuration::getInstance()->getPauseButton()) && (!slot->getPauseDisabled()) ) {
        ui->pauseButton->setVisible(true);
    }

    // m2: delete pause information (keyboard pause function)
    MainWindow::getInstance()->resetPause();
}

// m2:
void CartSlotWidget::updateSlotCounter(int newValue)
{
    // this when using a QLCDNumber instead of QLabel
    // ui->playedCounter->display(newValue);
    ui->playedCounter->setText(QString::number(newValue));
}

void CartSlotWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->formats().contains("application/emm.store.objectid") ||
        e->mimeData()->formats().contains("application/emm.store.slotid"))
        e->acceptProposedAction();
}

void CartSlotWidget::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->mimeData()->formats().contains("application/emm.store.objectid") ||
        e->mimeData()->formats().contains("application/emm.store.slotid"))
        e->accept();
}

void CartSlotWidget::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->formats().contains("application/emm.store.objectid") && e->proposedAction() == Qt::CopyAction) {
        int id = QString(e->mimeData()->data("application/emm.store.objectid")).toInt();
        slot->loadFromDatabase(id);
    } else if (e->mimeData()->formats().contains("application/emm.store.slotid") && e->proposedAction() == Qt::CopyAction) {
        int id = QString(e->mimeData()->data("application/emm.store.slotid")).toInt();
        slot->loadFromSlot(id);
    }
    showInfo();
    stoppedPlaying();
}

void CartSlotWidget::pauseSlot()
{
    slot->pause();
}
