INCLUDEPATH += $$PWD

HEADERS += \
            $$PWD/poly2tri.h \
            $$PWD/common/utils.h \
            $$PWD/common/shapes.h \
            $$PWD/sweep/advancing_front.h \
            $$PWD/sweep/cdt.h \
            $$PWD/sweep/sweep.h \
            $$PWD/sweep/sweep_context.h\

SOURCES += \
            $$PWD/common/shapes.cc \
            $$PWD/sweep/advancing_front.cc \
            $$PWD/sweep/cdt.cc \
            $$PWD/sweep/sweep.cc \
            $$PWD/sweep/sweep_context.cc

# $$PWD/test_poly2tri.cpp depends on other project. keep it separate from the library
