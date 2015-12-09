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

#include <QColor>
#include <QMimeData>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "model/audio/cartslot.h"
#include "slottablemodel.h"

extern bool DEMO;

SlotTableModel::SlotTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int SlotTableModel::rowCount(const QModelIndex &parent) const
{
    return slot.size();
}

int SlotTableModel::columnCount(const QModelIndex &parent) const
{
    return 6;
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
        case 3: return s->getText1();
        case 5: return s->getFileName();
        case 1: return s->getColor();
        case 2: return s->getFontColor();
        case 4: {
            int usedId = CartSlot::isUsed(s->getFileName());
            if (usedId == -1) {
                return "X";
            }
            return usedId;
        }
        }
    }
    else if (role == Qt::BackgroundRole || role == Qt::ForegroundRole)
    {
        // m2: changed order
        //if (index.column() > 2)
        if ( index.column() == 1 || index.column() == 2 )
        {
            CartSlot* s = slot.at(index.row());
            switch (index.column())
            {
            case 1: return QColor(s->getColor());
            case 2: return QColor(s->getFontColor());
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
        case 3: return "Name";
        case 5: return "Datei";
        // m2: changed labels
        case 1: return "HF";
        case 2: return "SF";
        //case 3: return "Farbe";
        //case 4: return "Textfarbe";
        case 4: return "SlotID";
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
    int i = 0;
    while (query.next())
    {
        CartSlot* s = CartSlot::getObjectWithNumber(query.value(0).toInt(),true);
        slotList.append(s);
        i++;
        // DEMO (max 60 entries)
        if (DEMO && i >= 60)
            break;
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
