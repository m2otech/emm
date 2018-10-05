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

#ifndef CARTSLOT_H
#define CARTSLOT_H

#include "abstractaudioobject.h"

class CartSlot : public AbstractAudioObject
{
    Q_OBJECT
public:
    QString getText1();
    bool getFadeOut();
    bool getLetFade();
    bool getFadeOthers();
    bool getLoop();
    double getStartPos();
    double getStopPos();
    int getPitch();
    // m2: set pitch
    void setPitch(int);
    int getFontSize();
    double getDB();
    // m2: set DB (volume)
    void setDB(double);
    int getNumber();
	double getLength();
    bool getEQActive();
    QString getEQConfig();
    void loadFromDatabase(int id);
    void loadFromSlot(int id);
    void empty();
    void clearColor();
	void fetchLength();
    void setDataAndSave(QString filename, QString text1, int type, int device, int channel, QString color, QString fontColor, bool fadeOut, bool letFade, bool fadeOthers, bool loop, double startPos, double stopPos, int pitch, int fontSize, double db, bool eqActive, QString eqConfig, bool pauseDisabled, bool cupEnabled);
    void setColorsAndSave(QString color, QString fontColor);
    void saveInDatabase();
    void loadEqualizer();
    static CartSlot* getObjectWithNumber(int number, bool db=false);
    static void fadeOutAllSlots(CartSlot *except=NULL, bool allowOthers = false);
    static int isUsed(QString filename);

    // m2: overwriting parent method to add fading
    void pause();

    // m2: new checkbox disable pause
    bool getPauseDisabled();

    // m2: new checkbox enable CUP
    bool getCupEnabled();

    // m2: get layer the slot belongs to (1-based)
    int getLayer();

    // m2: get slot length minus start position
    QString getTimeToPlay();

    // m2: set filename only
    void setFilename(QString);

private:
    CartSlot(int number, QObject *parent = 0, bool db=false);
    QString text1;
    bool fadeOutFlag;
    bool letFade;
    bool fadeOthers;
    bool loop;
    double startPos;
    double stopPos;
    int pitch;
    int fontSize;
    double db;
    double length;
    bool database;
    bool eqActive;
    QString eqConfig;
    static QMap<int,CartSlot*> audioObjects;
    // m2: new flag disable pause
    bool pauseDisabled;
    // m2: new flag enable CUP
    bool cupEnabled;

private slots:
    void readData();
    void saveData();

public slots:
    void play();
    void cartStop();

protected slots:
    virtual void updatePosition();

signals:
    void startedPlaying();
};

#endif // CARTSLOT_H
