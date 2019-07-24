TEMPLATE = app
TARGET = qmusicplayer

QT += multimedia multimediawidgets

HEADERS = \
    musicplayer.h

SOURCES = \
    main.cpp \
    musicplayer.cpp

INSTALLS += target

QT+=widgets

RESOURCES += \
    res.qrc
