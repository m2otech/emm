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

#include <QFileDialog>
#include <QTextStream>
#include <QTime>
#include "historylistwidget.h"
#include "model/playlistentry.h"

HistoryListWidget::HistoryListWidget(QWidget *parent) :
        QListWidget(parent)
{

}

void HistoryListWidget::addPlaylistItem(PlaylistEntry *entry)
{
    items.append(entry);
    QListWidgetItem *item = new QListWidgetItem(QTime::currentTime().toString("HH:mm:ss")+" : "+entry->getArtist()+" - "+entry->getTitle());
    this->addItem(item);
}

void HistoryListWidget::saveHistory()
{
    QStringList filters;
    filters << tr("M3U Playlisten (*.m3u)");
    QFileDialog dialog(this);
    dialog.setFont(QFont());
    dialog.setViewMode(QFileDialog::List);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilters(filters);
    dialog.setDefaultSuffix("m3u");
    if (dialog.exec())
    {
        if (dialog.selectedFiles().size()>0)
        {
            QString fileName = dialog.selectedFiles().at(0);
            QFile file(fileName);
            file.open(QIODevice::WriteOnly);
            QTextStream out(&file);
            out << "#EXTM3U\n";
            foreach(PlaylistEntry* entry, items)
            {
                out << "#EXTINF:" << qRound(entry->getLength()) << ",";
                out << entry->getArtist() << " - " << entry->getTitle() << "\n";
                out << entry->getFilename() << "\n";
            }
            file.close();
        }
    }
}
