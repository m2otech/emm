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

#include <QList>
#include <QUrl>
#include "SlotStoreImportThread.h"
#include "model/audio/cartslot.h"
#include "model/configuration.h"
#include "view/cartslotwidget.h"

#include "view/mainwindow.h"
#include "view/slotstoredialog.h"

SlotStoreImportThread::SlotStoreImportThread(QList<QUrl> urls, QObject *parent) :
    QThread(parent)
{
    this->urls = urls;
}

void SlotStoreImportThread::run() {
    // Get SS window
    MainWindow *win = MainWindow::getInstance();
    SlotStoreDialog *store = win->ssdGlobalThread;

    // Init progress bar
    emit updateMax(urls.size());

    // Add slots from dragged&dropped files
    int i = 1;
    foreach (const QUrl &url, urls) {
        QString fileName = url.toLocalFile();
        //qDebug() << "Dropped file:" << fileName;
        if (fileName.contains("."))
            store->addSlotAuto(fileName);
        emit updateStatus(i++);
    }
}


