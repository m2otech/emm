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

#include <QHeaderView>
#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <QKeyEvent>
#include <QFileDialog>
#include <QTextStream>
#include <math.h>
#include "model/audio/playlistplayer.h"
#include "model/playlist.h"
#include "model/playlistentry.h"
#include "model/playlistformats/lst.h"
#include "model/playlistformats/m3u.h"
#include "playlistwidget.h"

PlaylistWidget::PlaylistWidget(QWidget *parent) :
    RowMoveTableWidget(parent)
{
    //Header
    QStringList headerLabels;
    headerLabels << "Player" << "Artist" << "Titel" << "Länge";
    QList<QHeaderView::ResizeMode> headerSizes;
    headerSizes << QHeaderView::ResizeToContents << QHeaderView::Stretch << QHeaderView::Stretch << QHeaderView::ResizeToContents;
    setColumnCount(4);
    setHorizontalHeaderLabels(headerLabels);
    for (int i=0;i<columnCount();i++)
    {
        horizontalHeader()->setSectionResizeMode(i,headerSizes.at(i));
    }
    setDragDropMode(DragDrop);
    setAcceptDrops(true);
    this->playlist = Playlist::getInstance();
    connect(this, SIGNAL(itemsChanged()), playlist, SLOT(assignEntriesToPlayers()));
    connect(playlist, SIGNAL(playlistItemRemoved(int)), this, SLOT(removeRow(int)));
    connect(playlist, SIGNAL(playerAssigned(int,int)), this, SLOT(playerAssigned(int,int)));
}

bool PlaylistWidget::dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action)
{
    hideLine();
    if (data->hasUrls()) {
        clearSelection();
        for (int i=0;i<data->urls().size();i++) {
            QUrl url = data->urls().at(i);
            QString path = url.path().right(url.path().length()-1);
            QStringList suffix;
            suffix << "mp3" << "mp2" << "mp1" << "ogg" << "aiff" << "wav";
            if (QFileInfo(path).isDir()) {
                addDirectory(path);
            } else if (!suffix.contains(QFileInfo(path).suffix())) {
                continue;
            } else if (dropBeforeIndex != -1) {
                addFile(path);
            }
        }
    }
    dropBeforeIndex = draggedIndex = -1;
    dragging = false;
    return true;
}

void PlaylistWidget::addDirectory(QString path) {
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::Dirs);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
         QFileInfo fileInfo = list.at(i);
         if (fileInfo.isDir()) {
             if (fileInfo.fileName() == ".." || fileInfo.fileName() == ".")
                 continue;

             addDirectory(fileInfo.absoluteFilePath());
         } else {
             QStringList suffix;
             suffix << "mp3" << "mp2" << "mp1" << "ogg" << "aiff" << "wav";
             if (!suffix.contains(fileInfo.suffix()))
                 continue;
             addFile(fileInfo.absoluteFilePath());
         }
    }
}

void PlaylistWidget::addFile(QString path)
{
    insertRow(dropBeforeIndex);
    setItem(dropBeforeIndex,0,new QTableWidgetItem(""));
    setItem(dropBeforeIndex,1,new QTableWidgetItem(""));
    setItem(dropBeforeIndex,2,new QTableWidgetItem(""));
    setItem(dropBeforeIndex,3,new QTableWidgetItem(""));
    QFont font;
    font.setPixelSize(40);
    font.setBold(true);
    item(dropBeforeIndex,0)->setFont(font);
    item(dropBeforeIndex,0)->setTextAlignment(Qt::AlignCenter);

    PlaylistEntry *entry = playlist->addEntry(path,dropBeforeIndex);
    setItem(dropBeforeIndex,1,new QTableWidgetItem(entry->getArtist()));
    setItem(dropBeforeIndex,2,new QTableWidgetItem(entry->getTitle()));
    setItem(dropBeforeIndex,3,new QTableWidgetItem(formatSeconds(entry->getLength())));

    setCurrentItem(this->item(dropBeforeIndex,0));
    dropBeforeIndex++;
}

QStringList PlaylistWidget::mimeTypes() const
{
    QStringList returnList;
    returnList << "text/uri-list";
    return returnList;
}

void PlaylistWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete) {
        if (currentRow()>=0) {
            playlist->removeItemFromPlaylist(currentRow());
            removeRow(currentRow());
            emit itemsChanged();
        }
    } else {
        QTableWidget::keyPressEvent(e);
    }
}

QString PlaylistWidget::formatSeconds(double pos)
{
    int mins = pos/60;
    int secs = floor(pos-mins*60);
    return QString("%1:%2").arg(mins,2, 10, QChar('0')).arg(secs,2,10, QChar('0'));
}

void PlaylistWidget::playerAssigned(int row, int number)
{
    if (number==0)
    {
        this->item(row,0)->setText("");
    }
    else
    {
        this->item(row,0)->setText(QString::number(number));
    }
}

void PlaylistWidget::afterMove(int idx1, int idx2)
{
    playlist->move(idx1,idx2);
}

void PlaylistWidget::openPlaylist()
{
    QStringList filters;
    filters << tr("Unterstüzte Playlisten (*.m3u *.lst)");
    filters << tr("M3U Playlisten (*.m3u)");
    filters << tr("BPM-Studio Playlisten (*.lst)");
    QFileDialog dialog(this);
    dialog.setFont(QFont());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilters(filters);
    if (dialog.exec())
    {
        if (dialog.selectedFiles().size()>0)
        {
            QString fileName = dialog.selectedFiles().at(0);
            QStringList pl;
            QString format = fileName.right(3);
            if (format.toLower() == "m3u") {
                pl = M3U::loadEntriesFromFile(fileName);
            } else if (format.toLower() == "lst") {
                pl = LST::loadEntriesFromFile(fileName);
            }
            foreach(QString plRow, pl) {
                int pos = playlist->getEntries().size();
                insertRow(pos);
                setItem(pos,0,new QTableWidgetItem(""));
                setItem(pos,1,new QTableWidgetItem(""));
                setItem(pos,2,new QTableWidgetItem(""));
                setItem(pos,3,new QTableWidgetItem(""));
                QFont font;
                font.setPixelSize(40);
                font.setBold(true);
                item(pos,0)->setFont(font);
                item(pos,0)->setTextAlignment(Qt::AlignCenter);

                PlaylistEntry *entry = playlist->addEntry(plRow,pos);
                setItem(pos,1,new QTableWidgetItem(entry->getArtist()));
                setItem(pos,2,new QTableWidgetItem(entry->getTitle()));
                setItem(pos,3,new QTableWidgetItem(formatSeconds(entry->getLength())));
            }
        }
    }
}

void PlaylistWidget::savePlaylist()
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
            foreach(PlaylistEntry *entry, playlist->getEntries())
            {
                out << "#EXTINF:" << qRound(entry->getLength()) << ",";
                out << entry->getArtist() << " - " << entry->getTitle() << "\n";
                out << entry->getFilename() << "\n";
            }
            file.close();
        }
    }
}
