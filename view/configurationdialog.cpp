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

#include <QMessageBox>
#include "configurationdialog.h"
#include "model/configuration.h"
#include "model/layerdata.h"
#include "ui_configurationdialog.h"

#include <QDebug>

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);

    ui->menuTree->setCurrentItem(ui->menuTree->topLevelItem(0));

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(saveAndClose()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(close()));
    connect(ui->menuTree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    Configuration *config = Configuration::getInstance();
    ui->horizontalSpinBox->setValue(config->getHorizontalSlots());
    ui->verticalSpinBox->setValue(config->getVerticalSlots());
    ui->playerSpinBox->setValue(config->getPlayer());
    ui->playlistFadeSpinBox->setValue(config->getPlaylistFade());
    ui->slotFadeSpinBox->setValue(config->getSlotFade());
    ui->bufferSpinBox->setValue(config->getSlotBuffer());
    // m2: replace checkbox with combobox, same function
    ui->keyboardSyncCheckBox->setVisible(false);
    //ui->keyboardSyncCheckBox->setChecked(config->getLayerKeyboardSync());
    if (config->getLayerKeyboardSync())
        ui->keyboardSyncComboBox->setCurrentIndex(0);
    else
        ui->keyboardSyncComboBox->setCurrentIndex(1);
    // m2: pitch via keyboard checkbox
    ui->keyboardPitchCheckBox->setChecked(config->getPitchKeyboard());
    ui->playlistFadePosSpinBox->setValue(config->getPlaylistFPos());
    ui->layerSpinBox->setValue(config->getLayer());
    ui->deviceSelectWidget->setNewTitel(tr("PFL"));
    ui->deviceSelectWidget->selectDevice(config->getPFLDriver(),config->getPFLDevice(),config->getPFLChannel());
    ui->defaultDeviceWidget->setNewTitel(tr("Standard Audiogerät"));
    ui->defaultDeviceWidget->selectDevice(config->getDefaultDriver(),config->getDefaultDevice(),config->getDefaultChannel());
    ui->slotTimeSpinBox->setValue(config->getSlotTimeSize());
    ui->pauseCheckBox->setChecked(config->getPauseButton());

    // m2: Options for layer bar
    ui->showPauseButtonCheckBox->setChecked(config->getLayerbarPauseButton());
    ui->showStopButtonCheckBox->setChecked(config->getLayerbarStopButton());
    ui->showPitchControlCheckBox->setChecked(config->getLayerbarPitchControl());
    ui->showPlaylistButtonCheckBox->setChecked(config->getLayerbarPlaylistButton());
    ui->showRLACheckBox->setChecked(config->getLayerbarRLADisplay());

    // m2: remove this option for 2.2 (and set default to unchecked)
    ui->pauseCheckBox->setVisible(false);
    ui->pauseCheckBox->setChecked(false);
    ui->label_9->setVisible(false);

    QTreeWidgetItem *layerItem = ui->menuTree->topLevelItem(1);
    for (int i=0;i<config->getLayer();i++) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0,"Layer "+QString::number(i+1));
        layerItem->addChild(item);
    }
    layerItem->setExpanded(true);
    connect(ui->layerSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateLayerCount(int)));

    // m2: Layer add/remove buttons
    connect(ui->layerAddButton, SIGNAL(clicked()), this, SLOT(increaseLayerCount()));
    connect(ui->layerRemoveButton, SIGNAL(clicked()), this, SLOT(decreaseLayerCount()));

    connect(ui->showPauseButtonCheckBox, SIGNAL(clicked(bool)), this, SLOT(showRestartWarning(bool)));
    connect(ui->showPitchControlCheckBox, SIGNAL(clicked(bool)), this, SLOT(showRestartWarning(bool)));
    connect(ui->showPlaylistButtonCheckBox, SIGNAL(clicked(bool)), this, SLOT(showRestartWarning(bool)));
    connect(ui->showRLACheckBox, SIGNAL(clicked(bool)), this, SLOT(showRestartWarning(bool)));
    connect(ui->showStopButtonCheckBox, SIGNAL(clicked(bool)), this, SLOT(showRestartWarning(bool)));
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::closeWindow()
{
    done(0);
}

void ConfigurationDialog::saveAndClose()
{
    if (ui->horizontalSpinBox->value()*ui->verticalSpinBox->value()>109){
        QMessageBox::information(this,tr("Zu viele Slots"),tr("Es können maximal 109 Slots pro Layer angelegt werden."));
        return;
    }
    Configuration *config = Configuration::getInstance();
    config->setHorizontalSlots(ui->horizontalSpinBox->value());
    config->setVerticalSlots(ui->verticalSpinBox->value());
    config->setPlayer(ui->playerSpinBox->value());
    config->setPlaylistFade(ui->playlistFadeSpinBox->value());
    config->setSlotFade(ui->slotFadeSpinBox->value());
    //config->setLayerKeyboardSync(ui->keyboardSyncCheckBox->isChecked());
    config->setLayerKeyboardSync(ui->keyboardSyncComboBox->currentIndex() == 0);
    config->setPitchKeyboard(ui->keyboardPitchCheckBox->isChecked());
    config->setPlaylistFPos(ui->playlistFadePosSpinBox->value());
    config->setLayer(ui->layerSpinBox->value());
    config->setPFLDevice(ui->deviceSelectWidget->getDevice());
    config->setPFLDriver(ui->deviceSelectWidget->getDriver());
    config->setPFLChannel(ui->deviceSelectWidget->getChannel());
    config->setDefaultDevice(ui->defaultDeviceWidget->getDevice());
    config->setDefaultDriver(ui->defaultDeviceWidget->getDriver());
    config->setDefaultChannel(ui->defaultDeviceWidget->getChannel());
    config->setSlotBuffer(ui->bufferSpinBox->value());
    config->setSlotTimeSize(ui->slotTimeSpinBox->value());
    config->setPauseButton(ui->pauseCheckBox->isChecked());

    config->setLayerbarPauseButton(ui->showPauseButtonCheckBox->isChecked());
    config->setLayerbarStopButton(ui->showStopButtonCheckBox->isChecked());
    config->setLayerbarPitchControl(ui->showPitchControlCheckBox->isChecked());
    config->setLayerbarRLADisplay(ui->showRLACheckBox->isChecked());
    config->setLayerbarPlaylistButton(ui->showPlaylistButtonCheckBox->isChecked());

    config->saveData();
    done(1);
}

void ConfigurationDialog::selectionChanged() {
    int index = ui->menuTree->indexOfTopLevelItem(ui->menuTree->selectedItems().at(0));
    if (index == -1) {
        index = ui->menuTree->indexOfTopLevelItem(ui->menuTree->selectedItems().at(0)->parent());
        // Submenu under Layer selected
        if (index == 1) {
            ui->layerStack->setCurrentIndex(1);
            layerId = ui->menuTree->currentIndex().row();
            disconnect(ui->layerNameText,0,0,0);
            disconnect(ui->visibleCheckBox,0,0,0);
            Configuration *config = Configuration::getInstance();
            ui->layerNameText->setText(config->getLayers().value(layerId)->getName());
            ui->visibleCheckBox->setChecked(config->getLayers().value(layerId)->getVisible());
            connect(ui->layerNameText, SIGNAL(textChanged(QString)), this, SLOT(updateLayerName(QString)));
            connect(ui->visibleCheckBox, SIGNAL(clicked(bool)), this, SLOT(updateLayerEnabled(bool)));

            // Show general layer settings only when general layer tab is shown
            ui->label_13->setVisible(false);
            ui->showPauseButtonCheckBox->setVisible(false);
            ui->showPitchControlCheckBox->setVisible(false);
            ui->showPlaylistButtonCheckBox->setVisible(false);
            ui->showRLACheckBox->setVisible(false);
            ui->showStopButtonCheckBox->setVisible(false);
        }
    } else {
        // Main menu Layer selected
        if (index == 1) {
            ui->layerStack->setCurrentIndex(0);

            // Show general layer settings only when general layer tab is shown
            ui->label_13->setVisible(true);
            ui->showPauseButtonCheckBox->setVisible(true);
            ui->showPitchControlCheckBox->setVisible(true);
            ui->showPlaylistButtonCheckBox->setVisible(true);
            ui->showRLACheckBox->setVisible(true);
            ui->showStopButtonCheckBox->setVisible(true);
        }
    }
    ui->mainWindget->setCurrentIndex(index);
}

void ConfigurationDialog::increaseLayerCount() {
    ui->layerSpinBox->setValue(ui->layerSpinBox->value() + 1);
}

void ConfigurationDialog::decreaseLayerCount() {
    ui->layerSpinBox->setValue(ui->layerSpinBox->value() - 1);
}

void ConfigurationDialog::updateLayerCount(int count) {
    QTreeWidgetItem *layerItem = ui->menuTree->topLevelItem(1);
    Configuration *config = Configuration::getInstance();
    config->updateLayerCount(count);
    int preCount = layerItem->childCount();

    qDebug() << QString("Updated layer count: %1").arg(count);
    if (count > preCount) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0,"Layer "+QString::number(count));
        layerItem->addChild(item);
    } else {
        layerItem->removeChild(layerItem->child(count));
    }
}

void ConfigurationDialog::updateLayerEnabled(bool enabled) {
    Configuration *config = Configuration::getInstance();
    config->getLayers().value(layerId)->setVisible(enabled);
}

void ConfigurationDialog::updateLayerName(QString name) {
    Configuration *config = Configuration::getInstance();
    config->getLayers().value(layerId)->setName(name);
}

void ConfigurationDialog::showRestartWarning(bool checked) {
    QMessageBox::information(this,tr("Konfiguration aktualisiert"),tr("Nach dieser Änderung muss die EMM neu gestartet werden."));
}
