## Common for all libs
## do not call before setting target
CONFIG += C++11
QMAKE_CXXFLAGS_WARN_OFF += -Wunused-parameter
CONFIG(debug, debug|release): TARGET=$${TARGET}_debug
DESTDIR = $$PWD/lib/
