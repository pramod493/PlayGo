QT       += core gui widgets

TARGET = PlayGo

CONFIG   += console

include (../Core/PlayGoCore.pri)
include (../UI/PlayGoUI.pri)

SOURCES += $$PWD/PlayGoTestUI_Main.cpp
