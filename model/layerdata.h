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

#ifndef LAYERDATA_H
#define LAYERDATA_H

#include <QObject>

class LayerData : public QObject
{
    Q_OBJECT
public:
    explicit LayerData(int layerId, QObject *parent = 0);

public:
    int getLayerID();
    bool getVisible();
    QString getName();

    int getLayerPos();

    void setLayerPos(int);
    void setVisible(bool visible);
    void setName(QString name);

private:
    int layerId;
    bool visible;
    QString name;

    // layerPos is user-selectable (layerID is fixed)
    int layerPos;

};

#endif // LAYERDATA_H
