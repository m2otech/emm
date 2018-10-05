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

#ifndef TFCONTROLBAR_H
#define TFCONTROLBAR_H

#include <QWidget>

class QPushButton;
class QHBoxLayout;

class TFControlBar : public QWidget
{
    Q_OBJECT
public:
    explicit TFControlBar(QWidget *parent = 0);

private:
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *removeButton;
    QPushButton *playButton;
    QPushButton *stopButton;
    QPushButton *renameButton;

    QHBoxLayout *mainLayout;

private slots:
    void progessAdd();
    void progressEdit();
    void progessRemove();
    void processPlay();
    void processStop();
    void processRename();

signals:
    void addClicked();
    void editClicked();
    void removeClicked();
    void playClicked();
    void stopClicked();
    void renameClicked();
};

#endif // TFCONTROLBAR_H
