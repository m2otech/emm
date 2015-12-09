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

#ifndef DEVICESELECTWIDGET_H
#define DEVICESELECTWIDGET_H

#include <QWidget>

namespace Ui {
    class DeviceSelectWidget;
}

class QComboBox;
class QSpinBox;

class DeviceSelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceSelectWidget(QWidget *parent = 0);
    ~DeviceSelectWidget();
    void selectDevice(int driver, int device, int channel);
    int getDriver();
    int getDevice();
    int getChannel();
    void setNewTitel(QString title);

public slots:

private:
    Ui::DeviceSelectWidget *ui;

private slots:
     void updateChannel(int item);
     void showDevices(int driver);
};

#endif // DEVICESELECTWIDGET_H
