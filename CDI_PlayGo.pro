#-------------------------------------------------
#
# Project created by QtCreator 2015-04-05T00:29:47
#
#-------------------------------------------------

QT       += core gui widgets winextras

TARGET = PlayGo

CONFIG   += console

TEMPLATE = app

RESOURCES += images.qrc

include (PlayGoCore.pri)

include (P_Dollar.pri)

SOURCES += main.cpp \
    cdi2qtwrapper.cpp \
    tabletapplication.cpp \
    cdiwindow.cpp \
    cdigraphicspathitem.cpp \
    cdisearchgraphicsitem.cpp \
    sketchview.cpp \
    sketchscene.cpp

HEADERS += \
    cdi2qtwrapper.h \
    tabletapplication.h \
    cdiwindow.h \
    cdigraphicspathitem.h \
    uisettings.h \
    cdisearchgraphicsitem.h \
    sketchview.h \
    sketchscene.h
