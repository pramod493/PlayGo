QT       += core gui widgets

TARGET = PlayGoTest

CONFIG   += console

TEMPLATE = app

include (../Core/PlayGoCore.pri)

SOURCES += $$PWD/testpixmapmask.cpp
