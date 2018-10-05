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

#ifndef SLOTTABLEMODEL_H
#define SLOTTABLEMODEL_H

#include <QAbstractTableModel>

class CartSlot;
class PFLPlayer;

class SlotTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SlotTableModel(QObject *parent = 0);
    void loadData();
    void removeWithId(int id);
    void playWithId(int id);
    void stopWithId(int id);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::DropActions supportedDragActions() const;
    void replaceSlotFilename(QString, QString);

    int getIdPos(int);

private:
    QList<CartSlot*> slot;

    // m2
    PFLPlayer* player;

    QList<int> idList;

signals:

public slots:

};

#endif // SLOTTABLEMODEL_H
