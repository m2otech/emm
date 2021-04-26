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

#include <QAction>
#include <QList>
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
    // remove a layer
    if (count<this->getActionCount())
    {
        for(int i=this->getActionCount()-1;i>=count;i--)
        {
            this->removeButtonAt(i);
        }
    }
    // add a layer
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
    // populate current layers
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

    // Change the displayed layer order based on layerPos (from configuration dialog)
    setLayerOrder();
}

void LayerSelector::setLayerOrder()
{
    Configuration *conf = Configuration::getInstance();

    QMap<int, LayerData*> layers = conf->getLayers();

    layers_allocated.clear();

    for (int i=0;i<this->getActionCount();i++)
    {
        //QList<LayerData*> *layersOrdered = new QList<LayerData*>;
        int first_layer = i;

        int first_pos = 99;
        for (int k = 0; k < conf->getLayers().count(); k++)
        {
            LayerData *data = conf->getLayers().value(k);
            int pos = data->getLayerPos();
            if ( (pos < first_pos) && (!layers_allocated.contains(k)) )
            {
                first_layer = k;
                first_pos = pos;
            }
        }

        LayerData *data = conf->getLayers().value(first_layer);
        layers_allocated.append(first_layer);

        QAction *act = this->actions.at(i);
        act->setText(data->getName());
        act->setCheckable(true);
        act->setVisible(data->getVisible());
    }
}

QList<int> LayerSelector::getLayerOrder()
{
    return this->layers_allocated;
}

int LayerSelector::getSelectedButton()
{
    int ph_selected =  TFSelectionBar::getSelectedButton();

    return this->layers_allocated.at(ph_selected);
}
