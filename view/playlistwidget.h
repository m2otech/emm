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

#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include "rowmovetablewidget.h"

class Playlist;

class PlaylistWidget : public RowMoveTableWidget
{
    Q_OBJECT
public:
    explicit PlaylistWidget(QWidget *parent = 0);

protected:
    virtual bool dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action);
    QStringList mimeTypes() const;
    void keyPressEvent(QKeyEvent *event);
    void afterMove(int idx1, int idx2);

private:
    QString formatSeconds(double secs);
    void addFile(QString path);
    void addDirectory(QString path);
    Playlist* playlist;

public slots:
    void playerAssigned(int row, int number);
    void openPlaylist();
    void savePlaylist();

};

#endif // PLAYLISTWIDGET_H
