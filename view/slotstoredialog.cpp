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

#include <QFile>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "editcartslotdialog.h"
#include "model/slottablemodel.h"
#include "slotstoredialog.h"
#include "ui_slotstoredialog.h"

SlotStoreDialog::SlotStoreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SlotStoreDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Tool);
    model = new SlotTableModel();
    model->loadData();
    sortModel = new QSortFilterProxyModel();
    sortModel->setSourceModel(model);
    ui->storeTable->setModel(sortModel);
    // m2: changed order
    ui->storeTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->storeTable->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->storeTable->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Stretch);
    ui->storeTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    ui->storeTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    ui->storeTable->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Fixed);
    ui->storeTable->horizontalHeader()->resizeSection(1,25);
    ui->storeTable->horizontalHeader()->resizeSection(2,25);
    ui->storeTable->horizontalHeader()->resizeSection(4,70);
    connect(ui->controlBar, SIGNAL(addClicked()), this, SLOT(addSlot()));
    connect(ui->controlBar, SIGNAL(editClicked()), this, SLOT(editSlot()));
    connect(ui->controlBar, SIGNAL(removeClicked()), this, SLOT(removeSlot()));
    connect(ui->storeTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editSlot(QModelIndex)));
}

SlotStoreDialog::~SlotStoreDialog()
{
    delete ui;
}

void SlotStoreDialog::addSlot()
{
    EditCartSlotDialog *ecsd = new EditCartSlotDialog(-1,true);
    if (ecsd->exec())
    {
        model->loadData();
    }
}

void SlotStoreDialog::editSlot()
{
    QModelIndex index = sortModel->index(ui->storeTable->currentIndex().row(),0);
    editSlot(index);
}

void SlotStoreDialog::editSlot(QModelIndex index)
{
    if (!index.isValid())
        return;
    index = sortModel->mapToSource(index);
    int row = index.row();
    QModelIndex idx = model->index(row,0);
    int id = model->data(idx,Qt::DisplayRole).toInt();
    EditCartSlotDialog *ecsd = new EditCartSlotDialog(id,true);
    if (ecsd->exec())
    {
        model->loadData();
    }
}

void SlotStoreDialog::removeSlot()
{
    QModelIndex index = sortModel->index(ui->storeTable->currentIndex().row(),0);
    if (!index.isValid())
        return;
    index = sortModel->mapToSource(index);
    int id = model->data(index,Qt::DisplayRole).toInt();
    model->removeWithId(id);
    model->loadData();
}
