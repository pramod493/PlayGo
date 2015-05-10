#-------------------------------------------------
#
# Project created by QtCreator 2013-04-02T18:03:45
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = Indexer

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

#------------------------------------------------------------
#       WINDOWS SPECIFIC OPTIONS
INCLUDEPATH += $$PWD/boost_1_58_0/
INCLUDEPATH += $$PWD/tbb/include/
INCLUDEPATH += $$PWD/opencv/build/include/

# Note - These are only for release mode only. Add If/Else for release/debug mode
LIBS += "-L$$PWD/boost_1_58_0/stage/lib/x64" -llibboost_system-vc120-mt-1_58 -llibboost_filesystem-vc120-mt-1_58 -llibboost_program_options-vc120-mt-1_58
LIBS += "-L$$PWD/tbb/lib/intel64/vc12/" -ltbb
LIBS += "-L$$PWD/opencv/build/x64/vc12/lib" -lopencv_ts300 -lopencv_world300
#------------------------------------------------------------

#Add source files
SOURCES += indexer-main.cpp \
    wb2dshapedescriptor.cpp \
    wbbice.cpp \
    wbsearchengine.cpp \
    wbshape.cpp \
    wbmatcher.cpp

#Add header files
HEADERS += \
    parallelbice.h \
    wb2dshapedescriptor.h \
    wbbice.h \
    wbsearchengine.h \
    wbshape.h \
    wbmatcher.h
