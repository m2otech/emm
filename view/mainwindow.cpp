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
#include <QFileDialog>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>
#include <QTimer>
#include <QWheelEvent>
#include <math.h>
#include <QString>
#include "aboutdialog.h"
#include "bassmix.h"
#include "cartslotwidget.h"
#include "configurationdialog.h"
#include "mainwindow.h"
#include "model/audio/audioprocessor.h"
#include "model/audio/bassasiodevice.h"
#include "model/audio/bassdevice.h"
#include "model/audio/cartslot.h"
#include "model/audio/playlistplayer.h"
#include "model/audio/pluginloader.h"
#include "model/clearlayerthread.h"
#include "model/resetpitchesthread.h"
#include "model/configuration.h"
#include "model/copycolorsthread.h"
#include "model/globaldata.h"
#include "model/keyboardcontroller.h"
#include "model/playlist.h"
#include "playerwidget.h"
#include "slotstoredialog.h"
#include "slottablewidget.h"
#include "ui_mainwindow.h"

#include "../globals.h"

#include "model/audio/pflplayer.h"

#include <QCloseEvent>

#include <QMutex>

MainWindow* MainWindow::instance = 0;

// "Global" Store-Dialog (to be shown/hidden via keyboard)
SlotStoreDialog* ssdGlobal = NULL;
// "Global" Store-Dialog (to be used in parallel thread)
SlotStoreDialog* ssdGlobalThread = NULL;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    this->pauseModifier = false;
}

MainWindow::~MainWindow()
{
    AudioProcessor::getInstance()->freeDevices();
    AudioProcessor::dropInstance();
    KeyboardController::getInstance()->closeKeyboardController();
    delete ui;
}

void MainWindow::init() {
    ui->setupUi(this);

    Globals *globals = new Globals();
    this->setWindowTitle("Event Music Machine " + globals->getVersion());

    // m2: Point MainWindow::instance to correct location after initializing UI
    this->instance = this;

    // m2: Allocate and init all slot counters and pause info, init prevLayer (no layer)
    numberOfSlots = getTotalNumberOfSlots();
    playedCtr = (int*)calloc(numberOfSlots, sizeof(int));
    pausedSlot = (int*)calloc(numberOfSlots, sizeof(int));
    prevLayer = -1;

    // m2: Removed switch bar slots/playlist
    //ui->menuWidget->addButton(ui->slotAction);
    //ui->menuWidget->addButton(ui->playlistAction);
    //connect(ui->menuWidget, SIGNAL(currentTabChanged(int)),ui->stackedWidget,SLOT(setCurrentIndex(int)));
    //ui->menuWidget->selectFirstButton();

    createPlayers();

    fileModel = new QFileSystemModel();
    fileModel->setNameFilters(GlobalData::getSupportedAudioFormats(false));
    fileModel->setRootPath("/");
    fileModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    fileModel->setNameFilterDisables(false);
    //fileModel->setSupportedDragActions(Qt::CopyAction | Qt::MoveAction);
    ui->fileView->setModel(fileModel);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,9);
    ui->splitter_2->setStretchFactor(0,8);
    ui->splitter_2->setStretchFactor(1,2);
    this->showMaximized();
    connect(ui->configAction, SIGNAL(triggered()), this, SLOT(showConfigDialog()));
    connect(ui->aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(ui->autoPlayCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setAutoPlay()));
    connect(ui->playlistNextButton, SIGNAL(clicked()), Playlist::getInstance(), SLOT(fadeNext()));
    connect(ui->openButton, SIGNAL(clicked()), ui->playListTable, SLOT(openPlaylist()));
    connect(ui->saveButton, SIGNAL(clicked()), ui->playListTable, SLOT(savePlaylist()));
    connect(ui->layerSelector, SIGNAL(currentTabChanged(int)), this, SLOT(updateSlotAssignment()));
    connect(ui->saveHistoryButton, SIGNAL(clicked()), ui->historyList, SLOT(saveHistory()));
    connect(ui->slotStoreAction, SIGNAL(triggered()), this, SLOT(showSlotStore()));
    connect(ui->colorAction, SIGNAL(triggered()), this, SLOT(copyColors()));
    connect(ui->clearLayerAction, SIGNAL(triggered()), this, SLOT(clearSlots()));
    connect(ui->resetCountersAction, SIGNAL(triggered()), this, SLOT(resetCounters()));
    connect(ui->setConfigurationAction, SIGNAL(triggered()), this, SLOT(setConfigDirectory()));
    connect(ui->pauseAllButton, SIGNAL(clicked()), this, SLOT(pauseSlots()));
    connect(ui->stopAllButton, SIGNAL(clicked()), this, SLOT(stopSlots()));

    // m2: Pitch buttons
    connect(ui->pitchDownButton, SIGNAL(clicked()), this, SLOT(pitchDown()));
    connect(ui->pitchUpButton, SIGNAL(clicked()), this, SLOT(pitchUp()));
    connect(ui->pitchResetButton, SIGNAL(clicked()), this, SLOT(pitchReset()));
    connect(ui->resetPitchesAction, SIGNAL(triggered()), this, SLOT(resetPitches()));

    // m2: Volume buttons (DISABLED IN 2.5.3)
    connect(ui->dbDownButton, SIGNAL(clicked()), this, SLOT(dbDown()));
    connect(ui->dbUpButton, SIGNAL(clicked()), this, SLOT(dbUp()));
    connect(ui->dbResetButton, SIGNAL(clicked()), this, SLOT(dbReset()));

    //connect(ui->resetVolumesAction, SIGNAL(triggered()), this, SLOT(resetVolumes()));

    // HIDE VOLUME BUTTONS (DISABLE)
    ui->dbResetButton->setVisible(false);
    ui->dbDownButton->setVisible(false);
    ui->dbUpButton->setVisible(false);

    connect(ui->switchToPlayer, SIGNAL(clicked()), this, SLOT(showPlayer()));
    connect(ui->switchToSlots, SIGNAL(clicked()), this, SLOT(showSlots()));

    // m2: Slot-Store popup
    ssdGlobal = new SlotStoreDialog(this);
    ssdGlobalThread = new SlotStoreDialog(this);

    // m2: Set "crossed arrows" always on and disable it to avoid bug #21
    ui->autoPlayCheckBox->setChecked(true);
    ui->autoPlayCheckBox->setDisabled(true);

    // m2: show/hide layerbar buttons
    Configuration *config = Configuration::getInstance();
    ui->pauseAllButton->setVisible(config->getLayerbarPauseButton());
    ui->stopAllButton->setVisible(config->getLayerbarStopButton());
    ui->pitchDownButton->setVisible(config->getLayerbarPitchControl());
    ui->pitchResetButton->setVisible(config->getLayerbarPitchControl());
    ui->pitchUpButton->setVisible(config->getLayerbarPitchControl());
    ui->infoBox->setVisible(config->getLayerbarRLADisplay());
    ui->switchToPlayer->setVisible(config->getLayerbarPlaylistButton());

    // m2: !!!! DEBUG !!!!
    // to re-enable create a lineEdit text entry widget in main gui
    // int key = 999;
    // ui->lineEdit->setText(QString("Pressed key %1").arg(key, 0,'d',0));
}

void MainWindow::createPlayers()
{
    Configuration *config = Configuration::getInstance();
    int layers = config->getLayer();
    ui->layerSelector->setLayerCount(layers);
    int currentLayer = ui->layerSelector->getSelectedButton();
    foreach(PlayerWidget *playerWidget,playerWidgets)
    {
        ui->playerLayout->removeWidget(playerWidget);
        disconnect(playerWidget,0,0,0);
        playerWidget->~PlayerWidget();
    }
    playerWidgets.clear();

    ui->slotTableWidget->updateSlots(currentLayer);

    for (int i=0;i<config->getPlayer();i++)
    {
        PlayerWidget *newWidget = new PlayerWidget(i+1);
        connect(newWidget,SIGNAL(addToHistory(PlaylistEntry*)),ui->historyList, SLOT(addPlaylistItem(PlaylistEntry*)));
        ui->playerLayout->addWidget(newWidget);
        playerWidgets.append(newWidget);
    }
}

void MainWindow::updateSlotAssignment()
{
    Configuration *config = Configuration::getInstance();
    int currentLayer = ui->layerSelector->getSelectedButton();
    ui->slotTableWidget->updateSlots(currentLayer);
}

void MainWindow::showConfigDialog()
{
    ConfigurationDialog *configDialog = new ConfigurationDialog(this);
    if (configDialog->exec()==1)
    {
        ui->layerSelector->selectFirstButton();
        createPlayers();
    }
}

void MainWindow::setAutoPlay()
{
    Playlist::getInstance()->setAutoPlay(ui->autoPlayCheckBox->isChecked());
}

void MainWindow::showBassErrorMessage(int code)
{
    QString message="";
    switch (code)
    {
    case 3: message = tr("Der Treiber ist nicht verfügbar. Eventuell wird das Audiogerät schon benutzt."); break;
    case 8: message = tr("Ein Ausgabekanal konnte nicht angewählt werden, weil er nicht initialisiert wurde. Entweder ist die entsprechende Soundkarte nicht angeschlossen oder die entsprechenden Funktionen (z.B. ASIO-Gerät) sind in ihrem Programm nicht aktiviert."); break;
    case 23: message = tr("Das ausgewählte Audiogerät ist nicht vorhanden."); break;
    default: message = QString::number(code);
    }
    showErrorMessage(message);
}

void MainWindow::showErrorMessage(QString message)
{
    QMessageBox::critical(this,tr("Fehler"),message);
}

void MainWindow::keyboardSignal(int key, int pressed)
{
    Configuration *conf = Configuration::getInstance();
    if (pressed==0) //pressed
    {
        //ui->lineEdit->setText(QString("Pressed key %1").arg(key, 0,'d',0));
        if ( key>=105 && key<=115) {
            int layer = key - 104;
            if (layer <= conf->getLayer())
                ui->layerSelector->selectButtonAt(layer-1);
        } else if ( key>=117 && key<=120) {
            int layer = key - 105;
            if (layer <= conf->getLayer())
                ui->layerSelector->selectButtonAt(layer-1);
        } else if (key==121) {
            // Switch to Playlist
            if (ui->stackedWidget->currentIndex() == 0)
                ui->stackedWidget->setCurrentIndex(1);
            else
                ui->stackedWidget->setCurrentIndex(0);
        } else if (key==122) {
            // Layer up
            int layer = ui->layerSelector->getSelectedButton();
            if (layer >= 1)
                ui->layerSelector->selectButtonAt(layer-1);
        } else if (key==123) {
            // Layer down
            int layer = ui->layerSelector->getSelectedButton();
            if (layer <= conf->getLayer()-2)
                ui->layerSelector->selectButtonAt(layer+1);
        } else if (key==124) {
            // Slot-Speicher an/aus
            //showSlotStore();
            if (ssdGlobal == NULL)
                ssdGlobal = new SlotStoreDialog(this);
            if (ssdGlobal->isVisible())
                ssdGlobal->hide();
            else
                ssdGlobal->show();
            //ui->menuWidget->selectButtonAt((ui->menuWidget->getSelectedButton()+1)%2);
        } else if (key==125) {
            pauseSlots();

            //ui->autoPlayCheckBox->setChecked(!ui->autoPlayCheckBox->isChecked());
        } else if (key==127) {
            // Playlist NEXT
            Playlist::getInstance()->fadeNext();
        } else if (key==126) {
            // Playlist PLAY (player 1)
            Playlist::getInstance()->playPlayer(1);
            //Playlist::getInstance()->doAutoPlay(1);
        } else if (key==128) {
            CartSlot::fadeOutAllSlots(NULL,true);
            PlaylistPlayer::fadeOutAllPlayers();
            PFLPlayer::getInstance()->stopCue();
        } else if ( key==101 && conf->getPitchKeyboard() ) {
            this->resetPitches(true);
        } else if ( key==102 && conf->getPitchKeyboard() ) {
            this->pitchUp();
        } else if ( key==103 && conf->getPitchKeyboard() ) {
            this->pitchDown();
        } else if ( key==104 && conf->getPitchKeyboard() ) {
            this->pitchReset();
        } else if (key<=(conf->getHorizontalSlots()*conf->getVerticalSlots())) {
            int key2 = key;
            CartSlot *slot = AudioProcessor::getInstance()->getCartSlotWithNumber(key2);
            if (conf->getLayerKeyboardSync()) {
                key2 += (conf->getHorizontalSlots()*conf->getVerticalSlots()*ui->layerSelector->getSelectedButton());
                slot = AudioProcessor::getInstance()->getCartSlotWithNumber(key2);
                // m2: no need to jump back to previous layer (layer not changed)
                prevLayer = -1;
            } else {
                if (!slot->isPlaying()) {
                    // m2: keep track of which layer was selected before jumping to layer 1
                    prevLayer = ui->layerSelector->getSelectedButton() + 1;
                    // if we are already on layer 1 we don't need to switch back to this layer
                    if (prevLayer == 1)
                        prevLayer = -1;
                    //qDebug("Setting: %d", prevLayer);
                }
                // m2: v2.3 do not jump to layer 1 anymore
                //ui->layerSelector->selectButtonAt(0);
            }
            // m2: moved up
            //CartSlot *slot = AudioProcessor::getInstance()->getCartSlotWithNumber(key2);
            if (pauseModifier) {
                slot->pause();
            } else {
                slot->play();
            }
            ui->stackedWidget->setCurrentIndex(0);
        //} else if (key >= 116 && key <= 121) {
         //   ui->layerSelector->selectButtonAt(key-116);
// Playlist -> disable
//        } else {
//            int id = floor((double)key-109-((double)key-110)/2);
//            if (key%2 ==0) //start
//            {
//                AudioProcessor::getInstance()->getPlayerWithNumber(id)->play();
//                ui->stackedWidget->setCurrentIndex(1);
//            }
//            else //stop
//            {
//                AudioProcessor::getInstance()->getPlayerWithNumber(id)->stop();
//            }
        }
    } else {
        if (key==127) {
            this->pauseModifier = false;
        }
    }
}

//m2: catch keyboard input
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //qDebug("GVADemo keyPressEvent %d", event->key());
    // ALT key on PC keyboard => stop all
    if (event->key() == 16777251)
        this->stopSlots();
}

// m2: pause or continue all running titles
void MainWindow::pauseSlots()
{
    //Configuration *conf = Configuration::getInstance();
    MainWindow* currInstance = this->getInstance();

    // Pause-Funktion -> wirkt auf laufende(n) Titel
    //int noOfSongs = (conf->getLayers().count()*conf->getHorizontalSlots()*conf->getVerticalSlots());
    int noOfSongs = currInstance->numberOfSlots;

    // Find slot which is currently playing or paused
    CartSlot *slot = NULL;
    for (int i = 0; i < noOfSongs; i++) {
        slot = AudioProcessor::getInstance()->getCartSlotWithNumber(i);

        // This to pause or resume first title is found to be playing/paused
        //if ( slot->isPlaying() || slot->isPaused() ) {
        //    slot->pause(); // pause() will either pause or continue
        //    break;
        //}

        // This to resume paused title(s) or pause playing title(s)
        // (ONLY TITLES PAUSED VIA THIS KEYBOARD FUNCTION ARE CONSIDERED)
        //pausedSlot[i]
        if (currInstance->pausedSlot[i] > 0) {
            // Resume title
            slot->pause();
            currInstance->pausedSlot[i] = 0;
            //qDebug("resumed %d", i);
        }
        else if ( slot->isPlaying() && !slot->getPauseDisabled()) {
            // Pause title
            slot->pause();
            currInstance->pausedSlot[i] = 1;
            //qDebug("paused %d", i);
        }
    }
}

// m2: stop all running titles
void MainWindow::stopSlots()
{
    CartSlot::fadeOutAllSlots(NULL,true);

    // Clear RLA queue on stop all (normally redundant)
    //this->infoBoxQueue.clear();
}

// m2: increase pitch by 1%
void MainWindow::pitchUp()
{
    this->pitchChange(1);
}

// m2: decrease pitch by 1%
void MainWindow::pitchDown()
{
    this->pitchChange(-1);
}

// m2: reset pitch to slot default (value set in slot edit dialog and stored on disk)
void MainWindow::pitchReset()
{
    this->pitchChange(0);
}

// m2: increase DB by 1%
void MainWindow::dbUp()
{
    this->dbChange(1);
}

// m2: decrease DB by 1%
void MainWindow::dbDown()
{
    this->dbChange(-1);
}

// m2: reset DB to slot default (value set in slot edit dialog and stored on disk)
void MainWindow::dbReset()
{
    this->dbChange(0);
}

// m2: change = 0 => set pitch to default value (stored in slot)
//     change <> 0 => set pitch to current pitch +- change
void MainWindow::pitchChange(int change)
{
    MainWindow* currInstance = this->getInstance();

    // Running slot(s)
    int noOfSongs = currInstance->numberOfSlots;

    // Find slot which is currently playing
    CartSlot *slot = NULL;
    for (int i = 0; i < noOfSongs; i++) {
        slot = AudioProcessor::getInstance()->getCartSlotWithNumber(i);
        int newPitch;
        if (slot->isPlaying()) {
            if (change == 0) {
                // reload value from disk
                slot->loadFromSlot(i);
                newPitch = slot->getPitch();
                // set it explicitly to apply immediately
                slot->setPitch(newPitch);
            }
            else {
                newPitch = slot->getPitch() + change;
                slot->setPitch(newPitch);
            }
            pitchDisplayUpdate(slot->getPitch());
        }
    }
}

// m2: change = 0 => set volume to default value (stored in slot)
//     change <> 0 => set volume to current volume +- change
void MainWindow::dbChange(double change)
{
    MainWindow* currInstance = this->getInstance();

    // Running slot(s)
    int noOfSongs = currInstance->numberOfSlots;

    // Find slot which is currently playing
    CartSlot *slot = NULL;
    for (int i = 0; i < noOfSongs; i++) {
        slot = AudioProcessor::getInstance()->getCartSlotWithNumber(i);
        int newDB;
        if (slot->isPlaying()) {
            if (change == 0) {
                // reload value from disk
                slot->loadFromSlot(i);
                newDB = slot->getDB();
                // set it explicitly to apply immediately
                slot->setDB(newDB);
            }
            else {
                newDB = slot->getDB() + change;
                slot->setDB(newDB);
            }
            volumeDisplayUpdate(slot->getDB());
        }
    }
}

// m2: reset all pitches in current layer
void MainWindow::resetPitches() {
    this->resetPitches(true);
}

void MainWindow::resetPitches(bool confirmDialog) {

    bool confirmed = false;
    if (confirmDialog) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, QString::fromUtf8("Pitch Zurücksetzen"), "Soll bei allen Slots im aktuellen Layer der Pitch zurückgesetzt werden?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
            confirmed = true;
    }

    if ( confirmed || !confirmDialog ) {

        // Version with parallel thread (less-blocking)
        ResetPitchesThread *clear = new ResetPitchesThread(ui->layerSelector->getSelectedButton());
        QProgressDialog *dia = new QProgressDialog(this);
        dia->setCancelButton(NULL);
        dia->setLabelText(tr("Pitches zurücksetzen...."));
        dia->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        connect(clear,SIGNAL(updateStatus(int)), dia, SLOT(setValue(int)));
        connect(clear,SIGNAL(updateMax(int)), dia, SLOT(setMaximum(int)));
        connect(dia,SIGNAL(canceled()), clear, SLOT(quit()));
        clear->start();
        dia->exec();

        // Version without parallel thread (blocking)
        /*CartSlot *slot = NULL;
        for (int i = getLayerFirstSlotId(); i < getLayerFirstSlotId() + getLayerNumberOfSlots(); i++) {
            slot = AudioProcessor::getInstance()->getCartSlotWithNumber(i);
            // reload value from disk
            slot->loadFromSlot(i);
            if (slot->isPlaying())
                pitchDisplayUpdate(slot->getPitch());
        }*/
    }
}

void MainWindow::pitchDisplayUpdate(int pitch)
{
    QString currPitch = QString("%1 \%").arg(pitch);
    ui->pitchResetButton->setText(currPitch);
}

void MainWindow::volumeDisplayUpdate(double db)
{
    QString currVol = QString("%1 db").arg(db);
    ui->dbResetButton->setText(currVol);
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    Configuration *conf = Configuration::getInstance();
    int numDegrees = e->delta() / 8;
    int numSteps = numDegrees / 15;
    int currentIndex = ui->layerSelector->getSelectedButton();
    int layerCount = conf->getLayer();
    if (numSteps > 0)
    {
        currentIndex = qMax(0,currentIndex-numSteps);
    }
    else
    {
        currentIndex = qMin(layerCount-1,currentIndex-numSteps);
    }
    ui->layerSelector->selectButtonAt(currentIndex);
}

void MainWindow::showAbout()
{
    AboutDialog *dia = new AboutDialog(this);
    dia->exec();
}

void MainWindow::showSlotStore()
{
    SlotStoreDialog *ssd = new SlotStoreDialog(this);
    ssd->show();

    //ssdGlobal = new SlotStoreDialog(this);
    //ssdGlobal->show();
}

void MainWindow::copyColors() {
    int res = QMessageBox::question(this,tr("Wirklich kopieren?"),tr("Sollen die Farben wirklich kopiert werden?"),QMessageBox::Yes,QMessageBox::No);
    if (res == QMessageBox::No) {
        return;
    }
    CopyColorsThread *copy = new CopyColorsThread(ui->layerSelector->getSelectedButton());
    QProgressDialog *dia = new QProgressDialog(this);
    dia->setCancelButton(NULL);
    dia->setLabelText(tr("Farben kopieren...."));
    dia->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    connect(copy,SIGNAL(updateStatus(int)), dia, SLOT(setValue(int)));
    connect(copy,SIGNAL(updateMax(int)), dia, SLOT(setMaximum(int)));
    connect(dia,SIGNAL(canceled()), copy, SLOT(quit()));
    copy->start();
    dia->exec();
}

void MainWindow::clearSlots() {
    int res = QMessageBox::question(this,tr("Wirklich löschen?"),tr("Sollen alle Slots in diesem Layer entfernt werden?"),QMessageBox::Yes,QMessageBox::No);
    if (res == QMessageBox::No) {
        return;
    }

    ClearLayerThread *clear = new ClearLayerThread(ui->layerSelector->getSelectedButton());
    QProgressDialog *dia = new QProgressDialog(this);
    dia->setCancelButton(NULL);
    dia->setLabelText(tr("Layer löschen...."));
    dia->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    connect(clear,SIGNAL(updateStatus(int)), dia, SLOT(setValue(int)));
    connect(clear,SIGNAL(updateMax(int)), dia, SLOT(setMaximum(int)));
    connect(dia,SIGNAL(canceled()), clear, SLOT(quit()));
    clear->start();
    dia->exec();
    int currentLayer = ui->layerSelector->getSelectedButton();
    ui->slotTableWidget->updateSlots(currentLayer);

    // m2: also reset slot counters when clearing layer slots
    this->resetCounters(false);
}

// m2: Function to reset all slot counters to 0
void MainWindow::resetCounters() {
    this->resetCounters(true);
}

void MainWindow::resetCounters(bool confirmDialog) {

    bool confirmed = false;
    if (confirmDialog) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, QString::fromUtf8("Zähler Zurücksetzen"), "Sollen alle Zähler im aktuellen Layer zurückgesetzt werden?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
            confirmed = true;
    }

    if ( confirmed || !confirmDialog ) {
        MainWindow* currInstance = this->getInstance();
        memset(&currInstance->playedCtr[getLayerFirstSlotId()], 0, sizeof(*playedCtr) * getLayerNumberOfSlots());
        updateSlotAssignment();
    }
}

// m2: Function to reset all pause info to 0 (when playing other slot)
void MainWindow::resetPause() {
    MainWindow* currInstance = this->getInstance();

    int noOfSongs = currInstance->numberOfSlots;

    // Find slot which is currently paused (via keyboard)
    CartSlot *slot = NULL;
    for (int i = 0; i < noOfSongs; i++) {
        slot = AudioProcessor::getInstance()->getCartSlotWithNumber(i);

        if (currInstance->pausedSlot[i] > 0) {
            slot->stop(slot->getNumber());
            slot->stopped();
            currInstance->pausedSlot[i] = 0;
            //qDebug("stopped %d", i);
        }
    }
}

// m2:
void MainWindow::resetLayer() {
    //qDebug("Restoring: %d", prevLayer);
    if ( prevLayer > 0 )
        gotoLayer(prevLayer);
}

// m2:
void MainWindow::gotoLayer(int layer) {
    this->ui->layerSelector->selectButtonAt(layer - 1);
}

// m2:
int MainWindow::getLayerFirstSlotId() {
    Configuration *conf = Configuration::getInstance();

    int layer = ui->layerSelector->getSelectedButton();

    int rows = conf->getVerticalSlots();
    int columns = conf->getHorizontalSlots();

    return (layer * rows * columns);
}

// m2:
int MainWindow::getCurrentLayer() {
    // Layer number 1-based
    return (ui->layerSelector->getSelectedButton() + 1);
}

// m2:
int MainWindow::getSlotLayer(int slotNo) {
    // Layer number 1-based, slot number 1-based
    //qDebug() << QString("Slot is in layer: %1").arg((int)(slotNo / this->getLayerNumberOfSlots()) + 1);
    return ((int)((slotNo - 1) / this->getLayerNumberOfSlots()) + 1);
}

// m2:
bool MainWindow::isSlotShown(int slotNo) {
    return (this->getSlotLayer(slotNo) == this->getCurrentLayer());
}

// m2:
int MainWindow::getLayerNumberOfSlots()
{
    Configuration *conf = Configuration::getInstance();

    int rows = conf->getVerticalSlots();
    int columns = conf->getHorizontalSlots();

    return (rows * columns);
}

// m2:
int MainWindow::getTotalNumberOfSlots()
{
    Configuration *conf = Configuration::getInstance();

    return conf->getLayer() * getLayerNumberOfSlots();
}

// m2: Function to increase counter
void MainWindow::increaseCounter(int counterNo)
{
    playedCtr[counterNo]++;
}

void MainWindow::setConfigDirectory()
{
    QFileDialog dia(this);
    dia.setViewMode(QFileDialog::List);
    dia.setFileMode(QFileDialog::Directory);
    dia.setAcceptMode(QFileDialog::AcceptOpen);
    dia.setOptions(QFileDialog::ShowDirsOnly);
    dia.setDirectory(QDir(Configuration::getStorageLocation()));
    dia.setWindowTitle(QObject::tr("Bitte selektieren Sie einen Ordner in dem die Konfigurations-Dateien abgelegt werden sollen."));
    if (dia.exec() == 1) {
        Configuration::setStorageLocation(dia.selectedFiles().at(0));
        QMessageBox::information(this,tr("Konfiguration aktualisiert"),tr("Bitte starten Sie das Programm neu um die neuen Daten zu laden."));
    }
}

// m2: added to hold state variables
MainWindow* MainWindow::getInstance()
{
    static QMutex mutex;
    if (!instance)
    {
        mutex.lock();

        if (!instance)
            instance = new MainWindow;

        mutex.unlock();
    }
    return instance;
}

// m2: added to hold state variables
void MainWindow::dropInstance()
{
    static QMutex mutex;
    mutex.lock();
    delete instance;
    instance = 0;
    mutex.unlock();
}

void MainWindow::setInfoBox(QString text)
{
    ui->infoBox->setText(text);
    ui->infoBoxPL->setText(text);
}

void MainWindow::setInfoBox_2(QString text)
{
    ui->infoBoxPL_2->setText(text);
}

// m2: updates the RLA (pos2 is the time left in song)
void MainWindow::updateCurrSongPosition(double pos2, int layerNo)
{    
    int mins2 = pos2/60;
    int secs2 = floor(pos2-mins2*60);
    int msecs2 = floor((pos2-mins2*60-secs2)*10);
    QString time = "";
    if (layerNo > -1000)
        // Slot in RLA
        time = QString("L%4 %1:%2.%3").arg(mins2, 2, 10, QChar('0')).arg(secs2,2,10, QChar('0')).arg(msecs2).arg(layerNo);//.arg(infoBoxQueue.size());
    else {
        // Playlist in RLA
        if (layerNo == -1001)
            time = QString("P1 %1:%2.%3").arg(mins2, 2, 10, QChar('0')).arg(secs2,2,10, QChar('0')).arg(msecs2);
        else if (layerNo == -1002)
            time = QString("P2 %1:%2.%3").arg(mins2, 2, 10, QChar('0')).arg(secs2,2,10, QChar('0')).arg(msecs2);
    }
    if (time.size() > 0)
        setInfoBox(time);
    //qDebug("" + QString("%1").arg(infoBoxQueue));

    if (RLA_RED_COLOR) {
        if (infoBoxQueue.size() > 1) {
            ui->infoBox->setStyleSheet("QLabel { color : red; }");
            ui->infoBoxPL->setStyleSheet("QLabel { color : red; }");
        }
        else {
            ui->infoBox->setStyleSheet("QLabel { color : black; }");
            ui->infoBoxPL->setStyleSheet("QLabel { color : black; }");
        }
    }

    double remaining = pos2 + Playlist::getInstance()->getRemainingLength();
    mins2 = remaining/60;
    secs2 = floor(remaining-mins2*60);
    msecs2 = floor((remaining-mins2*60-secs2)*10);
    time = "";
    if (layerNo < -1000) {
        // Playlist in RLA
        time = QString("TOT %1:%2.%3").arg(mins2, 2, 10, QChar('0')).arg(secs2,2,10, QChar('0')).arg(msecs2);
    }
    if (time.size() > 0)
        setInfoBox_2(time);
}

// m2: add/remove/get info about song to be in the RLA
void MainWindow::infoBoxAddToQueue(int slotNo)
{
    // add to array infoBoxQueue
    infoBoxQueue.append(slotNo);
    //qDebug() << QString("Added: %1").arg(slotNo);
    //qDebug() << QString("Queue size: %1").arg(infoBoxQueue.size());
}

void MainWindow::infoBoxRemoveFromQueue(int slotNo)
{
    // remove from array infoBoxQueue
    if ( !(infoBoxQueue.isEmpty()) && infoBoxQueue.contains(slotNo) ) {
        //for (int i = 0; i < infoBoxQueue.size(); i++)
            //qDebug() << QString("Queue[%1]: %2").arg(i).arg(infoBoxQueue.at(i));

        infoBoxQueue.removeAll(slotNo);
        //qDebug() << QString("Removed: %1").arg(slotNo);
    }
    //qDebug() << QString("Queue size: %1").arg(infoBoxQueue.size());
}

int MainWindow::infoBoxGetLast()
{
    // get last item added to the list
    if ( !(infoBoxQueue.isEmpty()) )
        return infoBoxQueue.last();
    else
        return -1;
}

// m2: Confirm exit when closing window with X
void MainWindow::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Wirklich beenden?",
                                                                tr("Soll das Programm wirklich beendet werden?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
    }
}

// m2: switch between Slots and Player view
void MainWindow::showPlayer()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::showSlots()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// m2:
void MainWindow::renameSlotsFilename(QString replaceWhat, QString replaceWith)
{
    //Configuration *conf = Configuration::getInstance();
    MainWindow* currInstance = this->getInstance();

    int noOfSongs = currInstance->numberOfSlots;

    CartSlot *slot = NULL;
    for (int i = 0; i < noOfSongs; i++) {
        slot = AudioProcessor::getInstance()->getCartSlotWithNumber(i);
        //slot->filename.replace(replaceWhat, replaceWith);
        QString oldFilename = slot->getFileName();
        if (oldFilename.contains(replaceWhat))
            slot->setFilename(oldFilename.replace(replaceWhat, replaceWith));
    }
}

// m2: remove pointer to ssdGlobal (slotstore instance)
void MainWindow::ssdGlobalDelete()
{
    ssdGlobal = NULL;
}
