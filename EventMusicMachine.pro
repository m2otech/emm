# ----------------------------------------------------------------------------
# This file is part of Event Music Machine.
#
# Event Music Machine is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Event Music Machine is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Event Music Machine. If not, see <http://www.gnu.org/licenses/>.
# ----------------------------------------------------------------------------

QT       += core gui sql widgets

TARGET = EventMusicMachine
TEMPLATE = app
CONFIG += x86
CONFIG += c++11

DESTDIR = bin
MOC_DIR += moc
RCC_DIR += rcc
UI_DIR += ui
OBJECTS_DIR += o

win32 {
    LIBS += -L$$PWD/lib/win
}
macx {
    LIBS += -L$$PWD/lib/mac
}
LIBS += -lbass -ltags -lbassasio -lbassmix -lbass_fx

INCLUDEPATH += lib/header

RC_FILE = emm.rc

SOURCES += main.cpp\
    view/exportlayerdialog.cpp \
    view/mainwindow.cpp \
    view/configurationdialog.cpp \
    model/audio/bassdevice.cpp \
    view/cartslotwidget.cpp \
    view/editcartslotdialog.cpp \
    model/audio/cartslot.cpp \
    model/configuration.cpp \
    view/playlistwidget.cpp \
    model/playlistentry.cpp \
    view/playerwidget.cpp \
    model/audio/abstractaudioobject.cpp \
    model/audio/playlistplayer.cpp \
    model/playlist.cpp \
    view/rowmovetablewidget.cpp \
    model/audio/bassasiodevice.cpp \
    model/audio/abstractbassdevice.cpp \
    view/editplayerdialog.cpp \
    view/deviceselectwidget.cpp \
    model/keyboardcontroller.cpp \
    model/globaldata.cpp \
    model/audio/audioprocessor.cpp \
    view/aboutdialog.cpp \
    view/audiovisualizer.cpp \
    model/audio/pflplayer.cpp \
    view/historylistwidget.cpp \
    model/audio/pluginloader.cpp \
    model/playlistformats/m3u.cpp \
    model/playlistformats/lst.cpp \
    view/tfselectionbar.cpp \
    view/tfabstracttoolbar.cpp \
    view/layerselector.cpp \
    view/slotstoredialog.cpp \
    model/slottablemodel.cpp \
    view/tfcontrolbar.cpp \
    view/loadslotdialog.cpp \
    view/splashscreen.cpp \
    model/copycolorsthread.cpp \
    model/clearlayerthread.cpp \
    model/layerdata.cpp \
    view/slottablewidget.cpp \
    model/resetpitchesthread.cpp \
    model/slotstoreimportthread.cpp \
    view/slotstorerenamedialog.cpp \
    globals.cpp \
    model/exporttitlesthread.cpp


HEADERS += view/mainwindow.h \
    view/configurationdialog.h \
    model/audio/bassdevice.h \
    view/cartslotwidget.h \
    view/editcartslotdialog.h \
    model/audio/cartslot.h \
    model/configuration.h \
    view/exportlayerdialog.h \
    view/playlistwidget.h \
    model/playlistentry.h \
    view/playerwidget.h \
    model/audio/abstractaudioobject.h \
    model/audio/playlistplayer.h \
    model/playlist.h \
    view/rowmovetablewidget.h \
    model/audio/bassasiodevice.h \
    model/audio/abstractbassdevice.h \
    view/editplayerdialog.h \
    view/deviceselectwidget.h \
    model/keyboardcontroller.h \
    model/globaldata.h \
    model/audio/audioprocessor.h \
    view/aboutdialog.h \
    view/audiovisualizer.h \
    model/audio/pflplayer.h \
    view/historylistwidget.h \
    model/audio/pluginloader.h \
    model/playlistformats/m3u.h \
    model/playlistformats/lst.h \
    view/tfselectionbar.h \
    view/tfabstracttoolbar.h \
    view/layerselector.h \
    view/slotstoredialog.h \
    model/slottablemodel.h \
    view/tfcontrolbar.h \
    view/loadslotdialog.h \
    view/splashscreen.h \
    model/copycolorsthread.h \
    model/clearlayerthread.h \
    model/layerdata.h \
    view/slottablewidget.h \
    model/resetpitchesthread.h \
    model/slotstoreimportthread.h \
    view/slotstorerenamedialog.h \
    config.h \
    globals.h \
    model/resetpitchesthread.h \
    model/exporttitlesthread.h


FORMS += view/mainwindow.ui \
    view/configurationdialog.ui \
    view/cartslotwidget.ui \
    view/editcartslotdialog.ui \
    view/exportlayerdialog.ui \
    view/playerwidget.ui \
    view/editplayerdialog.ui \
    view/deviceselectwidget.ui \
    view/aboutdialog.ui \
    view/slotstoredialog.ui \
    view/loadslotdialog.ui \
    view/slotstorerenamedialog.ui

RESOURCES += \
    view/icons.qrc \
    model/database.qrc

TRANSLATIONS = translations/emm_en.ts \
    translations/emm_de.ts

OTHER_FILES += \
    LICENSE \
    README.md \
    emm.rc \
    .gitignore
