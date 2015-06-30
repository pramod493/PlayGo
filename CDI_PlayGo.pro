#----------------------------------------------------------------------
# Pramod Kumar
# Purdue University
#----------------------------------------------------------------------
QT       += core gui widgets svg
#QT += winextras

TARGET = PlayGo

CONFIG   += console

TEMPLATE = app

RESOURCES += images.qrc

#DEFINES += CDI_DEBUG_MODE

include ($$PWD/QsLog/QsLog.pri)

include ($$PWD/Core/PlayGoCore.pri)

#include (P-Dollar-Recognizer/P_Dollar.pri)

include (UI/PlayGoUI.pri)

include (Controller/Controller.pri)

SOURCES += $$PWD/main.cpp \
        $$PWD/miscfunctions.cpp \
        $$PWD/converttopolygons.cpp

HEADERS += \
        $$PWD/converttopolygons.h
