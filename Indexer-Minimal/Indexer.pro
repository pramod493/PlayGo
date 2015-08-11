#-----------------------------------------------------------
# Indexer main project - this will use the default parameters 
# in indexer-main.cpp file
#-----------------------------------------------------------
QT       += core
QT       -= gui

TARGET = Indexer

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

include ($$PWD/Indexer.pri)

SOURCES += indexer-main.cpp 