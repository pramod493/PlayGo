QT += core gui widgets multimedia printsupport
TARGET = playgoui
TEMPLATE = lib
CONFIG += staticlib
win32 {
	INCLUDEPATH += C:/Boost/include/boost-1_60/
	INCLUDEPATH += C:/opencv/build/include/
	INCLUDEPATH += C:/tbb/include/
}
INCLUDEPATH += $$PWD/../Indexer-Minimal
INCLUDEPATH += $$PWD/../poly2tri
INCLUDEPATH += $$PWD/../Physics
INCLUDEPATH += $$PWD/../QsLog
INCLUDEPATH += $$PWD/../P-Dollar-Recognizer
INCLUDEPATH += $$PWD/../qCustomPlot
INCLUDEPATH += $$PWD/../Core

include ($$PWD/PlayGoUI.pri)
include ($$PWD/../Controller/Controller.pri)

include ($$PWD/../common.pri)
