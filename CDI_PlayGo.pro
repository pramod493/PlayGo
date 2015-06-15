#----------------------------------------------------------------------
# Pramod Kumar
# Purdue University
#----------------------------------------------------------------------
QT       += core gui widgets winextras svg

TARGET = PlayGo

CONFIG   += console

TEMPLATE = app

RESOURCES += images.qrc

include (Core/PlayGoCore.pri)

#include (P-Dollar-Recognizer/P_Dollar.pri)

include (UI/PlayGoUI.pri)

include (Controller/Controller.pri)

SOURCES += $$PWD/main.cpp \
        $$PWD/miscfunctions.cpp \
        $$PWD/converttopolygons.cpp

HEADERS += \
        $$PWD/converttopolygons.h
