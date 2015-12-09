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

#ifndef TFABSTRACTTOOLBAR_H
#define TFABSTRACTTOOLBAR_H

#include <QWidget>

class QVBoxLayout;
class QToolBar;
class QActionGroup;
class QSignalMapper;

class TFAbstractToolbar : public QWidget
{
    Q_OBJECT
public:
    explicit TFAbstractToolbar(QWidget *parent = 0);
    void addButton(QAction* action);
    void selectFirstButton();
    int getSelectedButton();
    void selectButtonAt(int index);
    QAction* getButtonAt(int index);

protected:
    QToolBar *toolbar;
    int getActionCount();
    void removeButtonAt(int pos);
    QList<QAction*> actions;

private:
    QVBoxLayout *mainLayout;
    QActionGroup *actionGroup;
    QSignalMapper *mapper;

private slots:
    void buttonTriggered(int index);

signals:
    void currentTabChanged(int index);


};

#endif // TFABSTRACTTOOLBAR_H
