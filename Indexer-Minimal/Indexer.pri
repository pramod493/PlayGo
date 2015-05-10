#------------------------------------------------------------
#       WINDOWS SPECIFIC OPTIONS
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/boost_1_58_0/
INCLUDEPATH += $$PWD/tbb/include/
INCLUDEPATH += $$PWD/opencv/build/include/

# Note - These are only for release mode only. Add If/Else for release/debug mode
LIBS += "-L$$PWD/boost_1_58_0/stage/lib/x64" -llibboost_system-vc120-mt-1_58 -llibboost_filesystem-vc120-mt-1_58 -llibboost_program_options-vc120-mt-1_58
LIBS += "-L$$PWD/tbb/lib/intel64/vc12/" -ltbb
LIBS += "-L$$PWD/opencv/build/x64/vc12/lib" -lopencv_ts300 -lopencv_world300
#------------------------------------------------------------

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
