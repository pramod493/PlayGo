QT += core gui widgets
INCLUDEPATH += $$PWD
TARGET = pdollar
TEMPLATE = lib
CONFIG += staticlib

include ($$PWD/P_Dollar.pri)
include ($$PWD/../common.pri)
