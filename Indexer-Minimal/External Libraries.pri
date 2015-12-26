unix {
    #-------------------------------------------------------------
    #       LINUX SPECIFIC OPTIONS
    LIBS += -lopencv_core -lopencv_features2d -lopencv_shape -lopencv_ts -lopencv_imgproc -lopencv_photo -lopencv_imgcodecs
    #LIBS += -lopencv_core -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc  -lopencv_nonfree -lopencv_photo
    LIBS += -lboost_filesystem -lboost_system
    LIBS += -ltbb
}

# The keyword for windows seems to have changed in the new version. Please check again
win32:contains(QMAKE_HOST.arch, x86_64) {
    #------------------------------------------------------------
    #       WINDOWS SPECIFIC OPTIONS
    INCLUDEPATH += $$PWD/boost_1_58_0/
    INCLUDEPATH += $$PWD/tbb/include/
    INCLUDEPATH += $$PWD/opencv/build/include/

    #release
    # Note - These are only for release mode only. Add If/Else for release/debug mode
    Release: LIBS += "-L$$PWD/boost_1_58_0/stage/lib/x64" -llibboost_system-vc120-mt-1_58 -llibboost_filesystem-vc120-mt-1_58 -llibboost_program_options-vc120-mt-1_58
    Release: LIBS += "-L$$PWD/tbb/lib/intel64/vc12/" -ltbb
    Release: LIBS += "-L$$PWD/opencv/build/x64/vc12/lib" -lopencv_ts300 -lopencv_world300
    #------------------------------------------------------------

    #debug
    # Note - These are only for release mode only. Add If/Else for release/debug mode
    Debug: LIBS += "-L$$PWD/boost_1_58_0/stage/lib/x64" -llibboost_system-vc120-mt-gd-1_58 -llibboost_filesystem-vc120-mt-gd-1_58 -llibboost_program_options-vc120-mt-gd-1_58
    Debug: LIBS += "-L$$PWD/tbb/lib/intel64/vc12/" -ltbb_debug
    Debug: LIBS += "-L$$PWD/opencv/build/x64/vc12/lib" -lopencv_ts300d -lopencv_world300d
}

#--------------------------------------------------------------
# Intel TBB libraries create issues when building for android
# Easiest way to install android opencv and tbb libraries is
# through NVidia android dev kit
#--------------------------------------------------------------
android {
    #-------------------------------------------------------------
    #       ANDROID SPECIFIC OPTIONS
    INCLUDEPATH += $$PWD/boost_1_58_0/
    INCLUDEPATH += $$PWD/tbb/include/
    INCLUDEPATH += $$PWD/opencv/build/include/

    LIBS += "-L/home/pramod/NVPACK/OpenCV-2.4.8.2-Tegra-sdk/sdk/native/libs/x86" -lopencv_core -lopencv_features2d -lopencv_ts -lopencv_imgproc -lopencv_photo -lopencv_objdetect
    LIBS += "-L/home/pramod/NVPACK/OpenCV-2.4.8.2-Tegra-sdk/sdk/native/3rdparty/libs/x86" -llibpng -llibjpeg
    LIBS += -lboost_filesystem -lboost_system
    LIBS += "-L/home/pramod/Dropbox/CDI_PlayGo-Final/Indexer-Minimal/Runtime_Libraries/tbb_android" -ltbb
}
