
TARGET = DrowsyDriverConsole
TEMPLATE = app

include(DrowsyDriverCore.pri)

SOURCES += \
    ConsoleMain.cpp \
    ConsoleIO.cpp \
    ConsoleMainwindow.cpp

HEADERS  += \
    ConsoleIO.h \
    ConsoleMainwindow.h

# using Qt creator IDE without Qt
CONFIG += console
CONFIG -= qt
# executable is not an app
CONFIG -= app_bundle
