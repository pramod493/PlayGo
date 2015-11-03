#-----------------------------------------------------
# Pramod Kumar
# pramod@gmail.com
# batchpolygonize interface
#-----------------------------------------------------

QT += core gui widgets

TARGET = batchpolygonize

CONFIG += console

RESOURCES += \
	images.qrc \
	overlayimages.qrc

include ($$PWD/Core/PlayGoCore.pri)

include ($$PWD/P-Dollar-Recognizer/P_Dollar.pri)

include ($$PWD/UI/PlayGoUI.pri)

include ($$PWD/Controller/Controller.pri)

include ($$PWD/Indexer-Minimal/External Libraries.pri)

SOURCES += \
			$$PWD/initbatchpolygonize.cpp \
			$$PWD/batchpolygonize.cpp

HEADERS += \
			$$PWD/batchpolygonize.h

CONFIG += c++11

QMAKE_CXXFLAGS_WARN_OFF += -Wunused-parameter

RC_FILE = PlayGoIcon.rc

FORMS += $$PWD/batchpolygonize.ui
