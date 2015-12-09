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

#ifndef ROWMOVETABLEWIDGET_H
#define ROWMOVETABLEWIDGET_H

#include <QTableWidget>

class RowMoveTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit RowMoveTableWidget(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hideLine();
    void showLine(int y);

protected:
    virtual bool dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    Qt::DropActions supportedDropActions() const;

    virtual void afterMove(int idx1, int idx2)=0;

    QWidget *line;
    QPoint lastPress;
    bool dragging;
    int draggedIndex, dropBeforeIndex;

signals:
    void itemsChanged();

};

#endif // ROWMOVETABLEWIDGET_H
