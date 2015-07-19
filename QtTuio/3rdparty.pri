# Paths
INCLUDEPATH += \
    $$PWD/3rdparty/oscpack \
    $$PWD/3rdparty/tuio

# Files
HEADERS += $$PWD/3rdparty/oscpack/ip/IpEndpointName.h \
    $$PWD/3rdparty/oscpack/ip/NetworkingUtils.h \
    $$PWD/3rdparty/oscpack/ip/PacketListener.h \
    $$PWD/3rdparty/oscpack/ip/TimerListener.h \
    $$PWD/3rdparty/oscpack/ip/UdpSocket.h \
    $$PWD/3rdparty/oscpack/osc/MessageMappingOscPacketListener.h \
    $$PWD/3rdparty/oscpack/osc/OscException.h \
    $$PWD/3rdparty/oscpack/osc/OscHostEndianness.h \
    $$PWD/3rdparty/oscpack/osc/OscOutboundPacketStream.h \
    $$PWD/3rdparty/oscpack/osc/OscPacketListener.h \
    $$PWD/3rdparty/oscpack/osc/OscPrintReceivedElements.h \
    $$PWD/3rdparty/oscpack/osc/OscReceivedElements.h \
    $$PWD/3rdparty/oscpack/osc/OscTypes.h \
    $$PWD/3rdparty/tuio/TuioClient.h \
    $$PWD/3rdparty/tuio/TuioContainer.h \
    $$PWD/3rdparty/tuio/TuioCursor.h \
    $$PWD/3rdparty/tuio/TuioListener.h \
    $$PWD/3rdparty/tuio/TuioObject.h \
    $$PWD/3rdparty/tuio/TuioPoint.h \
    $$PWD/3rdparty/tuio/TuioServer.h \
    $$PWD/3rdparty/tuio/TuioTime.h

SOURCES += $$PWD/3rdparty/tuio/TuioClient.cpp \
    $$PWD/3rdparty/tuio/TuioServer.cpp \
    $$PWD/3rdparty/tuio/TuioTime.cpp \
    $$PWD/3rdparty/oscpack/osc/OscTypes.cpp \
    $$PWD/3rdparty/oscpack/osc/OscOutboundPacketStream.cpp \
    $$PWD/3rdparty/oscpack/osc/OscReceivedElements.cpp \
    $$PWD/3rdparty/oscpack/osc/OscPrintReceivedElements.cpp
# files for Windows
win32:SOURCES += $$PWD/3rdparty/oscpack/ip/win32/NetworkingUtils.cpp \
    $$PWD/3rdparty/oscpack/ip/win32/UdpSocket.cpp
# files for Unix
unix:SOURCES += $$PWD/3rdparty/oscpack/ip/posix/NetworkingUtils.cpp \
    $$PWD/3rdparty/oscpack/ip/posix/UdpSocket.cpp
