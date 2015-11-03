# This is currently [WIP] on how to organize the qt project as SUBDIR in order to reduce the
# compile time

include ($$PWD/../common.pri)

#! include( ../common.pri ) {
#    error( "Couldn't find the common.pri file!" )
#}
include ($$PWD/PlayGoUI.pri)
