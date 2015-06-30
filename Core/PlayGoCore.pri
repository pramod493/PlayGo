include ($$PWD/../Indexer-Minimal/Indexer.pri)
include ($$PWD/../poly2tri/poly2tri.pri)
include ($$PWD/../Physics/QBox2D.pri)
include ($$PWD/../EdgeDetect/Edge Detect.pri)
#include ($$PWD/../QsLog/QsLog.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/commonfunctions.h \
    $$PWD/point2dpt.h \
    $$PWD/stroke.h \
    $$PWD/image.h \
    $$PWD/abstractmodelitem.h \
    $$PWD/component.h \
    $$PWD/assembly.h \
    $$PWD/playgo.h \
    $$PWD/fileio.h \
    $$PWD/ramerdouglaspeucker.h \
    $$PWD/polygon2d.h \
    $$PWD/asmopencv.h \
    $$PWD/physicsmanager.h \
    $$PWD/searchmanager.h \
    $$PWD/page.h \
    $$PWD/physicsbody.h \
    $$PWD/physicsjoint.h \
    $$PWD/playgocore.h \
    $$PWD/searchresult.h \
    $$PWD/physicsshape.h

SOURCES += \
    $$PWD/commonfunctions.cpp \
    $$PWD/point2dpt.cpp \
    $$PWD/stroke.cpp \
    $$PWD/image.cpp \
    $$PWD/abstractmodelitem.cpp \
    $$PWD/component.cpp \
    $$PWD/assembly.cpp \
    $$PWD/playgo.cpp \
    $$PWD/fileio.cpp \
    $$PWD/ramerdouglaspeucker.cpp \
    $$PWD/polygon2d.cpp \
    $$PWD/physicsmanager.cpp \
    $$PWD/searchmanager.cpp \
    $$PWD/page.cpp \
    $$PWD/physicsbody.cpp \
    $$PWD/physicsjoint.cpp \
    $$PWD/searchresult.cpp \
    $$PWD/physicsshape.cpp

INCLUDEPATH += $$PWD/Clipper
HEADERS += $$PWD/Clipper/clipper.hpp
SOURCES += $$PWD/Clipper/clipper.cpp
