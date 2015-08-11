QT       += core gui widgets

TARGET = PDollarRecognizer

CONFIG += console

TEMPLATE = app

include ($$PWD/P-Dollar-Recognizer/P_Dollar.pri)

SOURCES += $$PWD/P-Dollar-Recognizer/gestureMain.cpp
