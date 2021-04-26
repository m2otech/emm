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

#include "layerdata.h"

LayerData::LayerData(int layerId, QObject *parent) :
    QObject(parent)
{
    this->layerId = layerId;
}

int LayerData::getLayerID() {
    return this->layerId;
}

int LayerData::getLayerPos() {
    return this->layerPos;
}

void LayerData::setLayerPos(int position) {
    this->layerPos = position;
}

bool LayerData::getVisible() {
    return this->visible;
}

QString LayerData::getName() {
    return this->name;
}

void LayerData::setVisible(bool visible) {
    this->visible = visible;
}

void LayerData::setName(QString name) {
    this->name = name;
}
