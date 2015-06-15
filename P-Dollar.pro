QT       += core gui widgets winextras

TARGET = PDollarRecognizer

CONFIG += console

TEMPLATE = app

INCLUDEPATH += $$PWD/P-Dollar-Recognizer

#include (Core/PlayGoCore.pri)

#include (P-Dollar-Recognizer/P_Dollar.pri)

HEADERS += P-Dollar-Recognizer/gestureScene.h \
            P-Dollar-Recognizer/reconizerwindow.h \
    P-Dollar-Recognizer/pdollarrecognizer.h

SOURCES +=   P-Dollar-Recognizer/gestureMain.cpp \
            P-Dollar-Recognizer/gestureScene.cpp \
            P-Dollar-Recognizer/reconizerwindow.cpp \
    P-Dollar-Recognizer/pdollarrecognizer.cpp
FORMS += \
    P-Dollar-Recognizer/reconizerwindow.ui
