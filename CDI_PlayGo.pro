#----------------------------------------------------------------------
# Pramod Kumar
# Purdue University
#----------------------------------------------------------------------
QT       += core gui widgets svg multimedia

TARGET = PlayGo

CONFIG   += console

TEMPLATE = app

RESOURCES += images.qrc

include ($$PWD/QsLog/QsLog.pri)

include ($$PWD/Core/PlayGoCore.pri)

#include (P-Dollar-Recognizer/P_Dollar.pri)

include ($$PWD/UI/PlayGoUI.pri)

include ($$PWD/Controller/Controller.pri)

SOURCES += $$PWD/main.cpp \
        $$PWD/miscfunctions.cpp \
        $$PWD/converttopolygons.cpp \
        $$PWD/batchpolygonize.cpp
HEADERS += \
        $$PWD/converttopolygons.h \
        $$PWD/batchpolygonize.h

CONFIG += c++11

QMAKE_CXXFLAGS_WARN_OFF += -Wunused-parameter

RC_FILE = PlayGoIcon.rc

FORMS += \
    $$PWD/batchpolygonize.ui
