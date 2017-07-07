#-------------------------------------------------
#
# Project created by QtCreator 2017-06-29T09:18:09
#
#-------------------------------------------------

QT       += core gui sql network multimedia multimediawidgets quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = musicPlayer
TEMPLATE = app

CONFIG	+= qt warn_on

CONFIG += link_pkgconfig
GST_VERSION = 1.0

PKGCONFIG += \
    gstreamer-$$GST_VERSION \
    gstreamer-base-$$GST_VERSION \
    gstreamer-audio-$$GST_VERSION \
    gstreamer-video-$$GST_VERSION \
    gstreamer-pbutils-$$GST_VERSION


INCLUDEPATH +=$$PWD base
include(base/base.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    musicwidgets.cpp \
    topwidgets.cpp \
    ueventthread.cpp \
    global_value.cpp \
    inotifythread.cpp

HEADERS  += mainwindow.h \
    musicwidgets.h \
    topwidgets.h \
    global_value.h \
    ueventthread.h \
    inotifythread.h

#FORMS    += mainwindow.ui


INCLUDEPATH +=$$PWD player
include(player/player.pri)

INCLUDEPATH +=$$PWD bottomWidget
include(bottomWidget/bottomWidget.pri)

INCLUDEPATH +=$$PWD middleWidget
include(middleWidget/middleWidget.pri)

RESOURCES += \
    res_music.qrc \
    res_main.qrc



UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/Music
INSTALLS += target
