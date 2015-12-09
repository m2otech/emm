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

#include <QApplication>
#include <QFileInfo>
#include <QHeaderView>
#include <QMouseEvent>
#include <QUrl>
#include "rowmovetablewidget.h"

RowMoveTableWidget::RowMoveTableWidget(QWidget *parent) :
        QTableWidget(parent), dragging(false), draggedIndex(-1), dropBeforeIndex(-1)
{
    line = new QWidget(viewport());
    hideLine();
    line->setAutoFillBackground(true);
    QPalette pal = line->palette();
    pal.setColor(line->backgroundRole(), Qt::blue);
    line->setPalette(pal);
}

void RowMoveTableWidget::dragMoveEvent(QDragMoveEvent *e)
{
    if (dragging) {
        QTableWidgetItem *item = itemAt(e->pos());
        if (item) {
            dropBeforeIndex = indexFromItem(item).row();
            showLine(visualItemRect(item).top());
        } else if (e->pos().y() >= 0) {
            dropBeforeIndex = rowCount();
            showLine(visualItemRect(this->item(rowCount()-1,0)).bottom());
        } else {
            dropBeforeIndex = -1;
            hideLine();
        }
        scrollToItem(item);
        e->accept();
    } else {
        QTableWidget::dragMoveEvent(e);
    }
}

void RowMoveTableWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    if (dragging) {
        hideLine();
        dragging = false;
        event->accept();
    }
    QTableWidget::dragLeaveEvent(event);
}

void RowMoveTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    dragging = true;
    line->show();
    QTableWidget::dragEnterEvent(event);
}

void RowMoveTableWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        lastPress = e->pos();
        QTableWidgetItem *item = itemAt(e->pos());
        draggedIndex = item ? indexFromItem(item).row() : -1;
    }
    QTableWidget::mousePressEvent(e);
}

void RowMoveTableWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (dragging) {
        QTableWidgetItem *item = itemAt(e->pos());
        if (item) {
            dropBeforeIndex = indexFromItem(item).row();
            showLine(visualItemRect(item).top());
        } else if (e->pos().y() >= 0) {
            dropBeforeIndex = rowCount();
            showLine(visualItemRect(this->item(rowCount()-1,0)).bottom());
        } else {
            dropBeforeIndex = -1;
            hideLine();
        }
        scrollToItem(item);
        e->accept();
    } else if (e->buttons() == Qt::LeftButton
               && (e->pos() - lastPress).manhattanLength() >= QApplication::startDragDistance()) {
        line->show();
        dragging = true;
    } else {
        QTableWidget::mouseMoveEvent(e);
    }
}

void RowMoveTableWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (dragging) {
        hideLine();
        if (draggedIndex != -1 && dropBeforeIndex != -1) {
            QList<QTableWidgetItem*> items;
            for (int i=0;i<columnCount();i++) {
                items.append(takeItem(draggedIndex,i));
            }
            removeRow(draggedIndex);
            if (draggedIndex < dropBeforeIndex)
                --dropBeforeIndex;
            clearSelection();
            afterMove(draggedIndex, dropBeforeIndex);
            insertRow(dropBeforeIndex);
            for (int i=0;i<columnCount();i++) {
                setItem(dropBeforeIndex,i,items.at(i));
            }
        }
        setCurrentItem(this->item(dropBeforeIndex,0));
        dropBeforeIndex = draggedIndex = -1;
        dragging = false;
        emit itemsChanged();
        e->accept();
    } else {
        QTableWidget::mouseReleaseEvent(e);
    }
}

Qt::DropActions RowMoveTableWidget::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::TargetMoveAction;
}

void RowMoveTableWidget::hideLine()
{
    line->setGeometry(0, -100, 1, 1);
}

void RowMoveTableWidget::showLine(int y)
{
    line->setGeometry(0, y, viewport()->width(), 2);
}

bool RowMoveTableWidget::dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action)
{
    bool success = false;
    hideLine();
    dropBeforeIndex = draggedIndex = -1;
    dragging = false;
    return success;
}
