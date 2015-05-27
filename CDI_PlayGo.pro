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

include (Indexer-Minimal/Indexer.pri)

#include (P-Dollar-Recognizer/P_Dollar.pri)

include (Physics/QBox2D.pri)

include (UI/PlayGoUI.pri)

SOURCES += $$PWD/main.cpp
