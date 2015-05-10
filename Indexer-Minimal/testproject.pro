QT += core
QT -= gui

TARGET = test_lib

CONFIG += console 
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/boost_1_58_0/

INCLUDEPATH += $$PWD/tbb/include/
INCLUDEPATH += $$PWD/opencv/build/include/

#INCLUDEPATH += $$PWD/OpenCV/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/core/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/hal/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/imgproc/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/photo/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/video/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/features2d/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/objdetect/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/calib3d/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/imgcodecs/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/videoio/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/highgui/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/ml/include/
#INCLUDEPATH += $$PWD/OpenCV/modules/flann/include/

#    LIBS += "-LE:/Indexer-WIP/boost_1_58_0/stage/lib/x64" -llibboost_system-vc120-mt-gd-1_58 -llibboost_filesystem-vc120-mt-gd-1_58 -llibboost_program_options-vc120-mt-gd-1_58
#    LIBS += "-LE:/Indexer-WIP/tbb/lib/intel64/vc12/" -ltbb_debug

    LIBS += "-L$$PWD/boost_1_58_0/stage/lib/x64" -llibboost_system-vc120-mt-1_58 -llibboost_filesystem-vc120-mt-1_58 -llibboost_program_options-vc120-mt-1_58
    LIBS += "-L$$PWD/tbb/lib/intel64/vc12/" -ltbb
    LIBS += "-L$$PWD/opencv/build/x64/vc12/lib" -lopencv_ts300 -lopencv_world300

SOURCES += testmain.cpp
