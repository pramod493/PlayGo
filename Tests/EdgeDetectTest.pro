QT       += core gui widgets

TARGET = PlayGoTest

CONFIG   += console

TEMPLATE = app

include ($$PWD/../Indexer-Minimal/External Libraries.pri)
include ($$PWD/../EdgeDetect/Edge Detect.pri)


SOURCES += $$PWD/EdgeDetectTest.cpp
