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

//#include <QDebug>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include "bass.h"
#include "bass_fx.h"
#include "bassasiodevice.h"
#include "bassdevice.h"
#include "cartslot.h"
#include "model/configuration.h"
#include "playlistplayer.h"

#include "view/mainwindow.h"

QMap<int,CartSlot*> CartSlot::audioObjects = QMap<int,CartSlot*>();

CartSlot::CartSlot(int number, QObject *parent, bool db) :
    AbstractAudioObject(number, parent)
{
    this->number = number;
    this->database = db;
    readData();
    if (!db)
    {
        initDevice();
        audioObjects.insert(number,this);
        fetchLength();
    }
}

void CartSlot::fetchLength() {
    if (playing) return;
    this->loadStream();
    length = BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetLength(stream,BASS_POS_BYTE));
    this->unloadStream();
    if (length < 0) length = 0;
    emit sendSongLength(length);
}

QString CartSlot::getText1()
{
    return this->text1;
}

void CartSlot::play()
{
    if (this->isPlaying())
        this->cartStop();
    else
    {
        // m2: add to RLA
        MainWindow::getInstance()->infoBoxAddToQueue(this->getNumber());

        this->loadStream(db);
        if (this->loop)
            BASS_ChannelFlags(stream, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
        else
            BASS_ChannelFlags(stream, 0, BASS_SAMPLE_LOOP);
        BASS_ChannelSetAttribute(stream, BASS_ATTRIB_TEMPO, this->pitch);
        // m2: update pitch display to current value
        MainWindow::getInstance()->pitchDisplayUpdate(this->pitch);
        AbstractAudioObject::play();
        if (this->startPos>0)
            BASS_ChannelSetPosition(stream,BASS_ChannelSeconds2Bytes(stream,startPos),BASS_POS_BYTE);
        if (this->fadeOthers)
        {
            CartSlot::fadeOutAllSlots(this);
            PlaylistPlayer::fadeOutAllPlayers();
        }
        emit(startedPlaying());
    }
}

void CartSlot::cartStop()
{
    int fadeMs = Configuration::getInstance()->getSlotFade();
    if (this->fadeOutFlag)
        AbstractAudioObject::fadeOut(fadeMs, this->getNumber());
    else
        AbstractAudioObject::stop(this->getNumber());

    //qDebug() << QString("Stopped slot: %1").arg(this->getNumber());
}

// m2: overwriting parent method to add fading
void CartSlot::pause()
{
    int fadeMs = Configuration::getInstance()->getSlotFade();
    if (playing) {
        if (this->fadeOutFlag) {
            //qDebug("Calling fadeOutPause()");
            AbstractAudioObject::fadeOutPause(fadeMs);
        } else {
            //qDebug("Calling pause directly");
            AbstractAudioObject::pause();
        }
    } else {
        //qDebug("Calling pause directly, to restart");
        AbstractAudioObject::pause();
    }
}

bool CartSlot::getLetFade()
{
    return this->letFade;
}

bool CartSlot::getFadeOut()
{
    return this->fadeOutFlag;
}

bool CartSlot::getFadeOthers()
{
    return this->fadeOthers;
}

// m2: new checkbox disable pause
bool CartSlot::getPauseDisabled()
{
    return this->pauseDisabled;
}

// m2: new checkbox enable CUP (play when mouse down / stop on release)
bool CartSlot::getCupEnabled()
{
    return this->cupEnabled;
}

bool CartSlot::getLoop()
{
    return this->loop;
}

double CartSlot::getStartPos()
{
    return this->startPos;
}

double CartSlot::getStopPos()
{
    return this->stopPos;
}

int CartSlot::getPitch()
{
    return this->pitch;
}

void CartSlot::setPitch(int pitch)
{
    this->pitch = pitch;
    BASS_ChannelSetAttribute(stream, BASS_ATTRIB_TEMPO, this->pitch);
}

int CartSlot::getFontSize()
{
    return this->fontSize;
}

double CartSlot::getDB()
{
    return this->db;
}

int CartSlot::getNumber()
{
    return this->number;
}

bool CartSlot::getEQActive()
{
    return this->eqActive;
}

QString CartSlot::getEQConfig()
{
    return this->eqConfig;
}

double CartSlot::getLength() {
    return this->length;
}

void CartSlot::setDataAndSave(QString filename, QString text1, int type, int device, int channel, QString color, QString fontColor, bool fadeOut, bool letFade, bool fadeOthers, bool loop, double startPos, double stopPos, int pitch, int fontSize, double db, bool eqActive, QString eqConfig, bool disablePause, bool enableCup)
{
    this->filename = filename;
    this->text1 = text1;
    this->type = type;
    this->device = device;
    this->channel = channel;
    this->color = color;
    this->fontColor = fontColor;
    this->fadeOutFlag = fadeOut;
    this->fadeOthers = fadeOthers;
    this->letFade = letFade;
    this->loop = loop;
    this->startPos = startPos;
    this->stopPos = stopPos;
    this->pitch = pitch;
    this->fontSize = fontSize;
    this->db = db;
    this->eqActive = eqActive;
    this->eqConfig = eqConfig;
    // m2: new checkboxes disable pause / enable CUP
    this->pauseDisabled = disablePause;
    this->cupEnabled = enableCup;
    this->saveData();
    if (!database)
    {
        this->initDevice();
        this->loadStream();
    }
}

void CartSlot::setColorsAndSave(QString color, QString fontColor)
{
    this->color = color;
    this->fontColor = fontColor;
    this->saveData();
}

void CartSlot::readData()
{
    if (!this->database || this->number == -1)
    {
        Configuration *config = Configuration::getInstance();
        QSettings settings(Configuration::getStorageLocation() + "/slots.ini", QSettings::IniFormat);
        // m2: set UTF8 (for äöüå)
        settings.setIniCodec("UTF-8");
        device = settings.value("Slot"+QString::number(number)+"/Device",config->getDefaultDevice()).toInt();
        channel = settings.value("Slot"+QString::number(number)+"/Channel",config->getDefaultChannel()).toInt();
        type = settings.value("Slot"+QString::number(number)+"/Type",config->getDefaultDriver()).toInt();
        filename = settings.value("Slot"+QString::number(number)+"/File").toString();
        text1 = settings.value("Slot"+QString::number(number)+"/Text1").toString();
        color = settings.value("Slot"+QString::number(number)+"/Color",0).toString();
        fontColor = settings.value("Slot"+QString::number(number)+"/FontColor",0).toString();
        letFade = settings.value("Slot"+QString::number(number)+"/LetFade",true).toBool();
        fadeOutFlag = settings.value("Slot"+QString::number(number)+"/FadeOut",true).toBool();
        fadeOthers = settings.value("Slot"+QString::number(number)+"/FadeOthers",true).toBool();
        loop = settings.value("Slot"+QString::number(number)+"/Loop",false).toBool();
        startPos = settings.value("Slot"+QString::number(number)+"/Start",0).toDouble();
        stopPos = settings.value("Slot"+QString::number(number)+"/Stop",0).toDouble();
        pitch = settings.value("Slot"+QString::number(number)+"/Pitch",0).toInt();
        fontSize = settings.value("Slot"+QString::number(number)+"/FontSize",14).toInt();
        db = settings.value("Slot"+QString::number(number)+"/DB",0).toDouble();
        eqActive = settings.value("Slot"+QString::number(number)+"/EqualizerActive",false).toBool();
        eqConfig = settings.value("Slot"+QString::number(number)+"/EqualizerConfig","").toString();
        // m2: new checkboxes disable pause / enable CUP (default false)
        pauseDisabled = settings.value("Slot"+QString::number(number)+"/PauseDisabled",false).toBool();
        cupEnabled = settings.value("Slot"+QString::number(number)+"/CupEnabled",false).toBool();
    }
    else
    {
        QSqlQuery query;

        // m2: Check if field pause_disabled exists, if not create it
        query.prepare("SELECT pause_disabled FROM slots");
        if (!query.exec()) {
            // No field => create it
            query.prepare("ALTER TABLE slots ADD COLUMN pause_disabled INTEGER DEFAULT 0");
            query.exec();
        }

        // m2: Check if field cup_enabled exists, if not create it
        query.prepare("SELECT cup_enabled FROM slots");
        if (!query.exec()) {
            // No field => create it
            query.prepare("ALTER TABLE slots ADD COLUMN cup_enabled INTEGER DEFAULT 0");
            query.exec();
        }

        query.prepare("SELECT * FROM slots WHERE slot_id=?");
        query.bindValue(0,this->number);
        query.exec();
        query.next();
        filename = query.value(1).toString();
        text1 = query.value(2).toString();
        color = query.value(3).toString();
        fadeOutFlag = query.value(4).toBool();
        fadeOthers = query.value(5).toBool();
        letFade = query.value(6).toBool();
        loop = query.value(7).toBool();
        startPos = query.value(8).toDouble();
        stopPos = query.value(9).toDouble();
        pitch = query.value(10).toDouble();
        fontSize = query.value(11).toInt();
        db = query.value(12).toDouble();
        fontColor = query.value(13).toString();

        // m2: adding pause_disabled ("4th checkbox")
        pauseDisabled = query.value(14).toBool();

        // m2: adding cup_enabled ("5th checkbox")
        cupEnabled = query.value(15).toBool();

        eqActive = false;
        eqConfig = "";
    }
}

void CartSlot::saveData()
{
    if (!this->database)
    {
        QSettings settings(Configuration::getStorageLocation() + "/slots.ini", QSettings::IniFormat);
        settings.setValue("Slot"+QString::number(number)+"/Device",device);
        settings.setValue("Slot"+QString::number(number)+"/Channel",channel);
        settings.setValue("Slot"+QString::number(number)+"/Type",type);
        settings.setValue("Slot"+QString::number(number)+"/File",filename);
        settings.setValue("Slot"+QString::number(number)+"/Text1",text1);
        settings.setValue("Slot"+QString::number(number)+"/Color",color);
        settings.setValue("Slot"+QString::number(number)+"/FontColor",fontColor);
        settings.setValue("Slot"+QString::number(number)+"/LetFade",letFade);
        settings.setValue("Slot"+QString::number(number)+"/FadeOut",fadeOutFlag);
        settings.setValue("Slot"+QString::number(number)+"/FadeOthers",fadeOthers);
        settings.setValue("Slot"+QString::number(number)+"/Loop",loop);
        settings.setValue("Slot"+QString::number(number)+"/Start",startPos);
        settings.setValue("Slot"+QString::number(number)+"/Stop",stopPos);
        settings.setValue("Slot"+QString::number(number)+"/Pitch",pitch);
        settings.setValue("Slot"+QString::number(number)+"/FontSize",fontSize);
        settings.setValue("Slot"+QString::number(number)+"/DB",db);
        settings.setValue("Slot"+QString::number(number)+"/EqualizerActive",eqActive);
        settings.setValue("Slot"+QString::number(number)+"/EqualizerConfig",eqConfig);
        // m2: new checkboxes disable pause / enable CUP
        settings.setValue("Slot"+QString::number(number)+"/PauseDisabled",pauseDisabled);
        settings.setValue("Slot"+QString::number(number)+"/CupEnabled",cupEnabled);
    }
    else
    {
        QSqlQuery query;

        // m2: Check if field pause_disabled exists, if not create it
        query.prepare("SELECT pause_disabled FROM slots");
        if (!query.exec()) {
            // No field => create it
            query.prepare("ALTER TABLE slots ADD COLUMN pause_disabled INTEGER DEFAULT 0");
            query.exec();
        }

        // m2: Check if field cup_enabled exists, if not create it
        query.prepare("SELECT cup_enabled FROM slots");
        if (!query.exec()) {
            // No field => create it
            query.prepare("ALTER TABLE slots ADD COLUMN cup_enabled INTEGER DEFAULT 0");
            query.exec();
        }

        if (this->number == -1)
        {
            //query.prepare("INSERT INTO slots (filename,desc,color,fade_out,fade_others,let_fade,loop,start_pos,stop_pos,pitch,font_size,db,font_color) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)");
            // m2: adding pause_disabled / cup_enabled
            query.prepare("INSERT INTO slots (filename,desc,color,fade_out,fade_others,let_fade,loop,start_pos,stop_pos,pitch,font_size,db,font_color,pause_disabled,cup_enabled) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
        }
        else
        {
            //query.prepare("UPDATE slots SET filename=?,desc=?,color=?,fade_out=?,fade_others=?,let_fade=?,loop=?,start_pos=?,stop_pos=?,pitch=?,font_size=?,db=?,font_color=? WHERE slot_id=?");
            //query.bindValue(13,number);
            // m2: adding pause_disabled / cup_enabled
            query.prepare("UPDATE slots SET filename=?,desc=?,color=?,fade_out=?,fade_others=?,let_fade=?,loop=?,start_pos=?,stop_pos=?,pitch=?,font_size=?,db=?,font_color=?,pause_disabled=?,cup_enabled=? WHERE slot_id=?");
            query.bindValue(15,number);
        }
        query.bindValue(0,filename);
        query.bindValue(1,text1);
        query.bindValue(2,color);
        query.bindValue(3,fadeOutFlag);
        query.bindValue(4,fadeOthers);
        query.bindValue(5,letFade);
        query.bindValue(6,loop);
        query.bindValue(7,startPos);
        query.bindValue(8,stopPos);
        query.bindValue(9,pitch);
        query.bindValue(10,fontSize);
        query.bindValue(11,db);
        query.bindValue(12,fontColor);
        // m2: adding pause_disabled / cup_enabled
        query.bindValue(13,pauseDisabled);
        query.bindValue(14,cupEnabled);
        query.exec();
    }
}

CartSlot* CartSlot::getObjectWithNumber(int number, bool db)
{
    if (db)
    {
        return new CartSlot(number,0,db);
    }
    else
    {
        if (audioObjects.contains(number))
            return audioObjects.value(number);
        else
            return new CartSlot(number,0,db);
    }
}

void CartSlot::updatePosition()
{
    // m2: do not update position if title is paused (to avoid overwriting the color)
    if ( !this->isPaused() ) {
        if (BASS_ChannelGetPosition(stream,BASS_POS_BYTE)>BASS_ChannelSeconds2Bytes(stream,BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetLength(stream,BASS_POS_BYTE))-stopPos))
        {
            if (this->loop)
            {
                BASS_ChannelSetPosition(stream,BASS_ChannelSeconds2Bytes(stream,startPos),BASS_POS_BYTE);
            }
            else
            {
                stop();
            }
            return;
        }
        emit sendCurrentPosition(BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetPosition(stream,BASS_POS_BYTE)));
        if (this->loop)
            return;
        AbstractAudioObject::updatePosition(this->getNumber(), this->getLayer());
    }
}

void CartSlot::fadeOutAllSlots(CartSlot *except, bool allowOthers)
{
    int fadeMs = Configuration::getInstance()->getSlotFade();
    QMapIterator<int, CartSlot*> i(audioObjects);
    while (i.hasNext()) {
        i.next();
        CartSlot *slot = i.value();
        if (except != NULL && slot == except)
            continue;
        if (slot->getLetFade() || allowOthers)
            slot->fadeOut(fadeMs);
    }
}

void CartSlot::saveInDatabase()
{
    CartSlot *tmp = this;
    int number = tmp->number;
    tmp->number = -1;
    tmp->database = true;
    tmp->saveData();
    tmp->number = number;
    tmp->database = false;
}

void CartSlot::loadFromDatabase(int id)
{
    int number = this->number;
    this->number = id;
    this->database = true;
    this->readData();
    this->database = false;
    this->number = number;
    this->saveData();
    this->fetchLength();
}

void CartSlot::loadFromSlot(int id)
{
    int number = this->number;
    this->number = id;
    this->readData();
    this->number = number;
    this->saveData();
    this->fetchLength();
}

void CartSlot::empty() {
    this->filename = "";
    this->startPos = 0;
    this->stopPos = 0;
    this->length = 0;
    this->text1 = "";
    // m2: also reset colors
    this->clearColor();
    this->loadStream();
    this->saveData();
    this->updatePosition();
}

void CartSlot::clearColor() {
    this->fontColor = "#000000";
    this->color = "#FFFFFF";

}

void CartSlot::loadEqualizer()
{
    eqs.clear();
    if (eqActive) {
        QList<QString> gains = eqConfig.split(";");
        if (gains.size() == 10) {
            int center = 16000;
            for (int i=0;i<10;i++) {
                BASS_DX8_PARAMEQ eq;
                eq.fGain = gains.at(i).toInt();
                eq.fCenter = center;
                eq.fBandwidth = 12;
                eqs.append(eq);
                center = center / 2;
            }
        }
    }
}

int CartSlot::isUsed(QString filename)
{
    foreach(int key, audioObjects.keys())
    {
        if (audioObjects.value(key)->getFileName() == filename) {
            return key;
        }
    }
    return -1;
}

int CartSlot::getLayer()
{
    int slotsPerLayer = Configuration::getInstance()->getVerticalSlots() * Configuration::getInstance()->getHorizontalSlots();

    return (int)(this->getNumber() / slotsPerLayer) + 1;
}
