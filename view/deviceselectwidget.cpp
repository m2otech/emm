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

#include "deviceselectwidget.h"
#include "model/audio/bassasiodevice.h"
#include "model/audio/bassdevice.h"
#include "ui_deviceselectwidget.h"

DeviceSelectWidget::DeviceSelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceSelectWidget)
{
    ui->setupUi(this);
    connect(ui->deviceComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannel(int)));
    connect(ui->driverComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(showDevices(int)));
}

DeviceSelectWidget::~DeviceSelectWidget()
{
    delete ui;
}

void DeviceSelectWidget::showDevices(int driver)
{
    ui->deviceComboBox->clear();
    if (driver==1)
    {
        foreach(AbstractBassDevice *device,BassAsioDevice::getAllDevices())
        {
            ui->deviceComboBox->addItem(device->getName(),device->getDeviceID());
        }
    }
    else
    {
        foreach(AbstractBassDevice *device,BassDevice::getAllDevices())
        {
            ui->deviceComboBox->addItem(device->getName(),device->getDeviceID());
        }
    }
    updateChannel(ui->deviceComboBox->currentIndex());
}

void DeviceSelectWidget::updateChannel(int item)
{
    //If no audio card is available return
    if (ui->deviceComboBox->count() == 0)
        return;

    int deviceid = ui->deviceComboBox->itemData(item).toInt();
    if (deviceid==0 && ui->driverComboBox->currentIndex()==0)
    {
        ui->channelSpinBox->setMinimum(0);
        ui->channelSpinBox->setMaximum(0);
        ui->channelSpinBox->setValue(0);
        return;
    }
    ui->channelSpinBox->setValue(1);
    ui->channelSpinBox->setMinimum(1);
    if (ui->driverComboBox->currentIndex()==0)
        ui->channelSpinBox->setMaximum(BassDevice::getDeviceWithID(deviceid)->getChannelCount());
    else
        ui->channelSpinBox->setMaximum(BassAsioDevice::getDeviceWithID(deviceid)->getChannelCount());
}

void DeviceSelectWidget::selectDevice(int driver, int device, int channel)
{
    ui->driverComboBox->setCurrentIndex(driver);
    showDevices(driver);
    ui->deviceComboBox->setCurrentIndex(ui->deviceComboBox->findData(device));
    ui->channelSpinBox->setValue(channel);
}

int DeviceSelectWidget::getDriver()
{
    return ui->driverComboBox->currentIndex();
}

int DeviceSelectWidget::getDevice()
{
    return ui->deviceComboBox->itemData(ui->deviceComboBox->currentIndex()).toInt();
}

int DeviceSelectWidget::getChannel()
{
    return ui->channelSpinBox->value();
}

void DeviceSelectWidget::setNewTitel(QString title)
{
    ui->groupBox_3->setTitle(title);
}
