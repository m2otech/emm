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

#ifndef SLOTSTOREDIALOG_H
#define SLOTSTOREDIALOG_H

#include <QDialog>
#include <QModelIndex>

#include <QDropEvent>
#include <QUrl>
#include <QDebug>
#include <QMimeData>

namespace Ui {
    class SlotStoreDialog;
}

class SlotTableModel;
class QSortFilterProxyModel;
class PFLPlayer;

class SlotStoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SlotStoreDialog(QWidget *parent = 0);
    ~SlotStoreDialog();
    void addSlotAuto(QString);

private:
    Ui::SlotStoreDialog *ui;
    SlotTableModel *model;
    QSortFilterProxyModel *sortModel;
    PFLPlayer* player;

    QString title;
    //void addSlotAuto(QString);

private slots:
    void addSlot();
    void editSlot();
    void editSlot(QModelIndex);
    void removeSlot();
    void playSlot();
    void stopSlot();
    void renameSlot();
    void setTitle(QString);

protected:
    // m2: drag & drop files to window
    void dropEvent(QDropEvent *ev);
    void dragEnterEvent(QDragEnterEvent *ev);
};

#endif // SLOTSTOREDIALOG_H
