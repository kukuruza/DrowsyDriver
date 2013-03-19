DEFINES += DEFAULT_VIDEO_PATH=\\\"/Users/evgenytoropov/Documents/Xcode/DrowsyDriver/data/6.mpg\\\"

TARGET = DrowsyDriverQt
TEMPLATE = app

include(DrowsyDriverCore.pri)

SOURCES += \
    QtMainwindow.cpp \
    QtMain.cpp

HEADERS  += \
    QtOutStream.h \
    QtMainwindow.h

QT       += core gui
FORMS    += mainwindow.ui
