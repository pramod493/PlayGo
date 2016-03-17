#-----------------------------------------------------------
# Indexer main project - this will use the default parameters
# in indexer-main.cpp file
#-----------------------------------------------------------
QT       -= core gui

TARGET = indexer

TEMPLATE = lib
CONFIG += staticlib

win32 {
	INCLUDEPATH += C:/Boost/include/boost-1_60/
	INCLUDEPATH += C:/opencv/build/include/
	INCLUDEPATH += C:/tbb/include/
}
include ($$PWD/Indexer.pri)
#SOURCES += indexer-main.cpp

include ($$PWD/../common.pri)
