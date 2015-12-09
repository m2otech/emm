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

#include <QAction>
#include "layerselector.h"
#include "model/configuration.h"
#include "model/layerdata.h"

LayerSelector::LayerSelector(QWidget *parent) :
    TFSelectionBar(parent)
{
    this->setLayerCount(Configuration::getInstance()->getLayer());
    this->selectFirstButton();
}


void LayerSelector::setLayerCount(int count)
{
    if (count<this->getActionCount())
    {
        for(int i=this->getActionCount()-1;i>=count;i--)
        {
            this->removeButtonAt(i);
        }
    }
    else if (count>this->getActionCount())
    {
        for (int i=this->getActionCount();i<count;i++)
        {
            LayerData *data = Configuration::getInstance()->getLayers().value(i);
            QAction *act = new QAction(data->getName(),this);
            act->setCheckable(true);
            act->setVisible(data->getVisible());
            this->addButton(act);
        }
    } else {
        for (int i=0;i<this->getActionCount();i++)
        {
            LayerData *data = Configuration::getInstance()->getLayers().value(i);
            QAction *act = this->actions.at(i);
            act->setText(data->getName());
            act->setCheckable(true);
            act->setVisible(data->getVisible());
        }
    }
}
