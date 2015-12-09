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

#include <QSqlQuery>
#include "loadslotdialog.h"
#include "ui_loadslotdialog.h"

LoadSlotDialog::LoadSlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadSlotDialog)
{
    ui->setupUi(this);
    QSqlQuery query;
    query.prepare("SELECT slot_id, desc FROM slots ORDER BY desc");
    query.exec();
    while (query.next())
    {
        ui->slotComboBox->addItem(query.value(1).toString(),query.value(0).toInt());
    }
    connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(selectEntry()));
}

LoadSlotDialog::~LoadSlotDialog()
{
    delete ui;
}

void LoadSlotDialog::selectEntry()
{
    done(ui->slotComboBox->itemData(ui->slotComboBox->currentIndex()).toInt());
}
