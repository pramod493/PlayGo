QT       += core widgets gui network multimedia
TARGET = playgocore
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/../Indexer
INCLUDEPATH += $$PWD/../poly2tri
INCLUDEPATH += $$PWD/../Physics
INCLUDEPATH += $$PWD/../QsLog

win32 {
	INCLUDEPATH += C:/Boost/include/boost-1_60/
	INCLUDEPATH += C:/opencv/build/include/
	INCLUDEPATH += C:/tbb/include/
}

include ($$PWD/PlayGoCore.pri)

include ($$PWD/../common.pri)
