#----------------------------------------------------------------------
# Pramod Kumar
# Purdue University
#----------------------------------------------------------------------
QT       += core gui widgets multimedia network

# sensors, sql are not used
# remove printsupport option might break qplot library
QT      += printsupport sql

###############################################
#### NOT USED ##############
#############################################
TARGET = PlayGo

CONFIG   += console

TEMPLATE = app

RESOURCES += images.qrc \
	overlayimages.qrc
## ---------------------------------------------------------------------
## TUIO is a touch library. It compiles fine on windows but throws error
## when using on windows. You need to install separate drivers on windows
## which will generate touch input from TUIO output

#unix {
#include ($$PWD/QtTuio/qTUIO.pri)
#}
## ----------------------------------------------------------------------
include ($$PWD/externallib.pri)

SOURCES += $$PWD/main.cpp
#SOURCES += $$PWD/main.cpp \
#		$$PWD/miscfunctions.cpp \
#		$$PWD/converttopolygons.cpp \
#		$$PWD/loadmodel.cpp
#HEADERS += \
#		$$PWD/converttopolygons.h \
#		$$PWD/LoadModel.h

CONFIG += c++11

QMAKE_CXXFLAGS_WARN_OFF += -Wunused-parameter

RC_FILE = PlayGoIcon.rc
