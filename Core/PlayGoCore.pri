include ($$PWD/../Indexer-Minimal/Indexer.pri)
include ($$PWD/../poly2tri/poly2tri.pri)
include ($$PWD/../Physics/QBox2D.pri)
include ($$PWD/../EdgeDetect/Edge Detect.pri)
#include ($$PWD/../QsLog/QsLog.pri)

#DEFINES += CDI_DEBUG_DRAW_SHAPE

INCLUDEPATH += $$PWD

HEADERS += $$PWD/clipper.hpp \
    $$PWD/grabcut.h \
    $$PWD/cdiboxdebugdraw.h \
    $$PWD/forceitem.h \
    $$PWD/penstroke.h \
    $$PWD/scenebutton.h \
    $$PWD/cdcontactlistener.h
SOURCES += $$PWD/clipper.cpp \
    $$PWD/grabcut.cpp \
    $$PWD/forceitem.cpp \
    $$PWD/penstroke.cpp \
    $$PWD/scenebutton.cpp \
    $$PWD/cdcontactlistener.cpp

HEADERS += \
    $$PWD/ramerdouglaspeucker.h \
    $$PWD/asmopencv.h \
    $$PWD/commonfunctions.h \
    $$PWD/abstractmodelitem.h \
    $$PWD/cdi2qtwrapper.h \
    $$PWD/point2dpt.h 
	
SOURCES += \
    $$PWD/ramerdouglaspeucker.cpp  \
    $$PWD/commonfunctions.cpp  \
    $$PWD/abstractmodelitem.cpp  \
    $$PWD/cdi2qtwrapper.cpp  \
    $$PWD/point2dpt.cpp  
	
HEADERS += \
    $$PWD/physicsbody.h \
    $$PWD/physicsjoint.h \
    $$PWD/physicsshape.h \
    $$PWD/searchresult.h 
	
SOURCES += \
    $$PWD/physicsbody.cpp  \
    $$PWD/physicsjoint.cpp  \
    $$PWD/physicsshape.cpp  \
    $$PWD/searchresult.cpp 
	
HEADERS += \
    $$PWD/stroke.h \
    $$PWD/pixmap.h \
    $$PWD/polygon2d.h \
    $$PWD/component.h \
    $$PWD/assembly.h \
    $$PWD/page.h \
    $$PWD/playgo.h 
	
SOURCES += \
    $$PWD/stroke.cpp  \
    $$PWD/pixmap.cpp  \
    $$PWD/polygon2d.cpp  \
    $$PWD/component.cpp  \
    $$PWD/assembly.cpp  \
    $$PWD/page.cpp  \
    $$PWD/playgo.cpp
	
HEADERS += \
    $$PWD/fileio.h \
    $$PWD/physicsmanager.h \
    $$PWD/searchmanager.h \
    $$PWD/playgocore.h \
    $$PWD/filesystemwatcher.h

SOURCES += \
    $$PWD/fileio.cpp  \
    $$PWD/physicsmanager.cpp  \
    $$PWD/searchmanager.cpp \
    $$PWD/filesystemwatcher.cpp

