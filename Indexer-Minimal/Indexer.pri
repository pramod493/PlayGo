INCLUDEPATH += $$PWD

include ($$PWD/External Libraries.pri)

#Add source files
SOURCES += \
    $$PWD/wb2dshapedescriptor.cpp \
    $$PWD/wbbice.cpp \
    $$PWD/wbsearchengine.cpp \
    $$PWD/wbshape.cpp \
    $$PWD/wbmatcher.cpp

#Add header files
HEADERS += \
    $$PWD/parallelbice.h \
    $$PWD/wb2dshapedescriptor.h \
    $$PWD/wbbice.h \
    $$PWD/wbsearchengine.h \
    $$PWD/wbshape.h \
    $$PWD/wbmatcher.h
