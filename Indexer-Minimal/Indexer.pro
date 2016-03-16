#-----------------------------------------------------------
# Indexer main project - this will use the default parameters
# in indexer-main.cpp file
#-----------------------------------------------------------
QT       -= core gui

CONFIG(debug, debug|release): TARGET = indexerd
CONFIG(release, debug|release): TARGET = indexer

TEMPLATE = lib
CONFIG += staticlib

include ($$PWD/../opencv.pri)
include ($$PWD/../boost.pri)
include ($$PWD/../tbb.pri)

include ($$PWD/Indexer.pri)

#SOURCES += indexer-main.cpp
DESTDIR = $$PWD/../lib/
CONFIG += c++11
QMAKE_CXXFLAGS_WARN_OFF += -Wunused-parameter
