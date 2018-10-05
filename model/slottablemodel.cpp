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
#include <QColor>
#include <QMimeData>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "model/audio/cartslot.h"
#include "slottablemodel.h"

#include <math.h>

#include "model/audio/audioprocessor.h"
#include "model/audio/pflplayer.h"

SlotTableModel::SlotTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    // m2: Init player to be used to play preview
    player = AudioProcessor::getInstance()->getPFLPlayer();

}

int SlotTableModel::rowCount(const QModelIndex &parent) const
{
    return slot.size();
}

int SlotTableModel::columnCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant SlotTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        int row = index.row();
        CartSlot* s = slot.at(row);
        switch (index.column())
        {
        // m2: changed order
        case 0: return s->getNumber();
        case 4: return s->getText1();
        case 6: return s->getFileName();
        case 2: return s->getColor();
        case 3: return s->getFontColor();
        case 1: {
            int usedId = CartSlot::isUsed(s->getFileName());
            if (usedId == -1) {
                return "X";
            }
            return usedId;
        }
        case 5:
            QString time = s->getTimeToPlay();
            return time;
        }
    }
    else if (role == Qt::BackgroundRole || role == Qt::ForegroundRole)
    {
        // m2: changed order
        //if (index.column() > 2)
        if ( index.column() == 2 || index.column() == 3 )
        {
            CartSlot* s = slot.at(index.row());
            switch (index.column())
            {
            case 2: return QColor(s->getColor());
            case 3: return QColor(s->getFontColor());
            }
        }

    }
    return QVariant();
}

QVariant SlotTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        // m2: changed order
        case 0: return "ID";
        case 4: return "Name";
        case 6: return "Datei";
        // m2: changed labels
        case 2: return "HF";
        case 3: return "SF";
        //case 3: return "Farbe";
        //case 4: return "Textfarbe";
        case 1: return "SlotID";
        case 5: return "Laufzeit";
        }
    }
    return QVariant();
}

Qt::ItemFlags SlotTableModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

QMimeData* SlotTableModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.size() == 0)
        return 0;

    QMimeData *mimeData = new QMimeData();
    int id = data(index(indexes.at(0).row(),0),Qt::DisplayRole).toInt();
    QByteArray encodedData = QByteArray::number(id);
    mimeData->setData("application/emm.store.objectid",encodedData);
    return mimeData;
}

Qt::DropActions SlotTableModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

void SlotTableModel::loadData()
{
    this->beginResetModel();
    QSqlQuery query("SELECT slot_id FROM slots");
    QList<CartSlot*> slotList;
    idList.clear();
    while (query.next())
    {
        int id = query.value(0).toInt();
        CartSlot* s = CartSlot::getObjectWithNumber(id, true);
        //CartSlot* s = CartSlot::getObjectWithNumber(query.value(0).toInt(),true);
        //CartSlot* s2 = AudioProcessor::getInstance()->getCartSlotWithNumber(query.value(0).toInt());
        slotList.append(s);
        idList.append(id);
    }
    slot = slotList;

    this->endResetModel();
}

void SlotTableModel::removeWithId(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM slots WHERE slot_id=?");
    query.bindValue(0,id);
    query.exec();
}

void SlotTableModel::playWithId(int id)
{
    CartSlot* s = slot.at(id);
    // m2: get start position and play preview from there
    player->setFilename(s->getFileName());
    player->analyse();
    player->playCue(s->getStartPos());
}

void SlotTableModel::stopWithId(int id)
{
    // m2: id is not used right now, leaving it for future use
    player->stop();
}

int SlotTableModel::getIdPos(int id)
{
    return idList.indexOf(id);
}

// m2
void SlotTableModel::replaceSlotFilename(QString replaceWhat, QString replaceWith)
{
    //this->beginResetModel();

    QSqlQuery query("UPDATE slots SET filename = replace(filename, '" + replaceWhat + "', '" + replaceWith + "')");
    //qDebug() << QString("Called rename");

    // NB: The previous statement already executes the query, no need to exec
    // query.exec();

    //this->endResetModel();
}

