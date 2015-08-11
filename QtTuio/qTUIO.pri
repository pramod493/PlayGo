INCLUDEPATH += $$PWD

# Small configurations
CONFIG += precompile_header
win32:CONFIG += embed_manifest_exe
DEFINES += OSC_HOST_LITTLE_ENDIAN

# Windows libraries for OSC Pack
#win32:LIBS += ws2_32.lib \
#    winmm.lib

# External files for the project
include($$PWD/3rdparty.pri)

# Main project files
HEADERS += $$PWD/qtuio.h
SOURCES += $$PWD/qtuio.cpp

