#
# Pramod Kumar
#
#DEFINES += CDI_DEBUG_DRAW_SHAPE
# Enable when adding plots
INCLUDEPATH += $$PWD

HEADERS +=  \
	$$PWD/grabcut.h \
        $$PWD/cdiboxdebugdraw.h \
	$$PWD/cdcontactlistener.h \
	$$PWD/material.h
SOURCES +=  \
        $$PWD/grabcut.cpp \
	$$PWD/cdcontactlistener.cpp \
	$$PWD/material.cpp

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
	$$PWD/filesystemwatcher.h

SOURCES += \
	$$PWD/fileio.cpp  \
	$$PWD/physicsmanager.cpp  \
	$$PWD/searchmanager.cpp \
	$$PWD/filesystemwatcher.cpp

# Need this for gesture recognizer
HEADERS += penstroke.h
SOURCES += penstroke.cpp

HEADERS += $$PWD/FindContours.h
SOURCES += $$PWD/FindContours.cpp

HEADERS += $$PWD/sketchscene.h
SOURCES += $$PWD/sketchscene.cpp

HEADERS += forceitem.h
SOURCES += forceitem.cpp

