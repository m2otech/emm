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
#include "model/configuration.h"
#include "model/copycolorsthread.h"
#include "model/globaldata.h"
#include "model/keyboardcontroller.h"
#include "model/playlist.h"
#include "playerwidget.h"
#include "slotstoredialog.h"
#include "slottablewidget.h"
#include "ui_mainwindow.h"

#include <QMutex>

extern bool DEMO, DEMO_M;

MainWindow* MainWindow::instance = 0;

// "Global" Store-Dialog (to be shown/hidden via keyboard)
SlotStoreDialog* ssdGlobal = NULL;

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

    // m2: Point MainWindow::instance to correct location after initializing UI
    this->instance = this;

    // m2: Allocate and init all slot counters and pause info, init prevLayer (no layer)
    numberOfSlots = getTotalNumberOfSlots();
    playedCtr = (int*)calloc(numberOfSlots, sizeof(int));
    pausedSlot = (int*)calloc(numberOfSlots, sizeof(int));
    prevLayer = -1;

    // m2: Disabled playlist
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

    // DEMO Hide Pause All und Stop All buttons, disable Menu items
    if (DEMO)
    {
        ui->pauseAllButton->hide();
        ui->stopAllButton->hide();
        ui->resetCountersAction->setEnabled(false);
        ui->colorAction->setEnabled(false);
        ui->setConfigurationAction->setEnabled(false);
    }
    else if (DEMO_M)
    {
        ui->setConfigurationAction->setVisible(false);
        ui->colorAction->setVisible(false);
    }

    // m2: Slot-Store popup
    ssdGlobal = new SlotStoreDialog(this);

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
    case 8: message = tr("Ein Ausgabekanal konnte nicht angewählt werden, weil er nicht initialisert wurde. Entweder ist die entsprechende Soundkarte nicht angeschlossen oder die entsprechenden Funktionen (z.B. ASIO-Gerät) sind in ihrem Programm nicht aktiviert."); break;
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
        if (key>=105 && key<=115 && !DEMO_M) {
            int layer = key - 104;
            ui->layerSelector->selectButtonAt(layer-1);
        } else if (key>=117 && key<=120 && !DEMO_M) {
            int layer = key - 105;
            ui->layerSelector->selectButtonAt(layer-1);
        }
        if (key>=105 && key<=109 && DEMO_M) {
            int layer = key - 104;
            ui->layerSelector->selectButtonAt(layer-1);
        } else if (key==121) {
            // frei lassen Layer 16 wird nicht benötigt
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
            if (ssdGlobal->isVisible())
                ssdGlobal->hide();
            else
                ssdGlobal->show();
            //ui->menuWidget->selectButtonAt((ui->menuWidget->getSelectedButton()+1)%2);
        } else if (key==125) {
            pauseSlots();

            //ui->autoPlayCheckBox->setChecked(!ui->autoPlayCheckBox->isChecked());
        } else if (key==126) {
            // frei
            // Playlist::getInstance()->fadeNext();
        } else if (key==127) {
            // frei
            // this->pauseModifier = true;
        } else if (key==128) {
            CartSlot::fadeOutAllSlots(NULL,true);
            PlaylistPlayer::fadeOutAllPlayers();
        } else if (key<=(conf->getHorizontalSlots()*conf->getVerticalSlots())) {
            int key2 = key;
            CartSlot *slot = AudioProcessor::getInstance()->getCartSlotWithNumber(key2);
            if (conf->getLayerKeyboardSync()) {
                key2 += (conf->getHorizontalSlots()*conf->getVerticalSlots()*ui->layerSelector->getSelectedButton());
                // m2: update slot (in current layer)
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
                }
                ui->layerSelector->selectButtonAt(0);
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
            slot->stop();
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
    //qDebug("counterNo = %d", counterNo);
    //qDebug("playedCtr[] = %d", playedCtr[counterNo]);
    //qDebug("Instance: %d", instance);
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

