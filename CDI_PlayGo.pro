#----------------------------------------------------------------------
# Pramod Kumar
# Purdue University
#----------------------------------------------------------------------
QT       += core gui widgets multimedia network #webkitwidgets

# sensors, sql are not used
# remove printsupport option might break qplot library
QT      += sensors printsupport sql

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

include ($$PWD/Core/PlayGoCore.pri)

include ($$PWD/P-Dollar-Recognizer/P_Dollar.pri)

include ($$PWD/UI/PlayGoUI.pri)

include ($$PWD/Controller/Controller.pri)

include ($$PWD/opencv.pri)
include ($$PWD/boost.pri)
include ($$PWD/tbb.pri)

SOURCES += $$PWD/main.cpp \
		$$PWD/miscfunctions.cpp \
		$$PWD/converttopolygons.cpp \
		$$PWD/loadmodel.cpp
HEADERS += \
		$$PWD/converttopolygons.h \
		$$PWD/LoadModel.h

CONFIG += c++11

QMAKE_CXXFLAGS_WARN_OFF += -Wunused-parameter

RC_FILE = PlayGoIcon.rc
