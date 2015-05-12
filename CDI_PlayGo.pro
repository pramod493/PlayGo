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

include (P_Dollar.pri)

include (Indexer-Minimal/Indexer.pri)

SOURCES += $$PWD/main.cpp \
    $$PWD/cdi2qtwrapper.cpp \
    $$PWD/tabletapplication.cpp \
    $$PWD/cdiwindow.cpp \
    $$PWD/cdigraphicspathitem.cpp \
    $$PWD/cdisearchgraphicsitem.cpp \
    $$PWD/sketchview.cpp \
    $$PWD/sketchscene.cpp \
    $$PWD/gesturemanager.cpp \
    $$PWD/searchmanager.cpp \
    $$PWD/colorselectortoolbar.cpp

HEADERS += \
    $$PWD/cdi2qtwrapper.h \
    $$PWD/tabletapplication.h \
    $$PWD/cdiwindow.h \
    $$PWD/cdigraphicspathitem.h \
    $$PWD/uisettings.h \
    $$PWD/cdisearchgraphicsitem.h \
    $$PWD/sketchview.h \
    $$PWD/sketchscene.h \
    $$PWD/gesturemanager.h \
    $$PWD/searchmanager.h \
    $$PWD/colorselectortoolbar.h
