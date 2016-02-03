unix {
    #-------------------------------------------------------------
    #       LINUX SPECIFIC OPTIONS
    LIBS += -lopencv_core -lopencv_features2d -lopencv_shape -lopencv_ts -lopencv_imgproc -lopencv_photo -lopencv_imgcodecs
    #LIBS += -lopencv_core -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc  -lopencv_nonfree -lopencv_photo
    LIBS += -lboost_filesystem -lboost_system
    LIBS += -ltbb
}

# The keyword for windows seems to have changed in the new version. Please check again
win32 {
    #------------------------------------------------------------
    #       WINDOWS SPECIFIC OPTIONS
    INCLUDEPATH += C:/Boost/include/boost-1_60/
    INCLUDEPATH += C:/tbb/include/
    INCLUDEPATH += C:/opencv/build/include/

    #release
    # Note - These are only for release mode only. Add If/Else for release/debug mode
    Release: LIBS += -LC:/Boost/lib/ -llibboost_system-vc140-mt-1_60 -llibboost_filesystem-vc140-mt-1_60 -llibboost_program_options-vc140-mt-1_60
    Release: LIBS += -LC:/tbb/lib/intel64/vc14/ -ltbb
    Release: LIBS += -LC:/opencv/build/x64/vc14/lib -lopencv_world310
    #------------------------------------------------------------

    #debug
    # Note - These are only for release mode only. Add If/Else for release/debug mode
    Debug: LIBS += -LC:/Boost/lib/ -llibboost_system-vc140-mt-gd-1_60 -llibboost_filesystem-vc140-mt-gd-1_60 -llibboost_program_options-vc140-mt-gd-1_60
    Debug: LIBS += -LC:/tbb/lib/intel64/vc14/ -ltbb_debug
    Debug: LIBS += "-LC:/opencv/build/x64/vc14/lib" -lopencv_world310d
}

#--------------------------------------------------------------
# Intel TBB libraries create issues when building for android
# Easiest way to install android opencv and tbb libraries is
# through NVidia android dev kit
#--------------------------------------------------------------
#android {
#    #-------------------------------------------------------------
#    #       ANDROID SPECIFIC OPTIONS
#    INCLUDEPATH += $$PWD/boost_1_58_0/
#    INCLUDEPATH += $$PWD/tbb/include/
#    INCLUDEPATH += $$PWD/opencv/build/include/
#
#    LIBS += "-L/home/pramod/NVPACK/OpenCV-2.4.8.2-Tegra-sdk/sdk/native/libs/x86" -lopencv_core -lopencv_features2d -lopencv_ts -lopencv_imgproc -lopencv_photo -lopencv_objdetect
#    LIBS += "-L/home/pramod/NVPACK/OpenCV-2.4.8.2-Tegra-sdk/sdk/native/3rdparty/libs/x86" -llibpng -llibjpeg
#    LIBS += -lboost_filesystem -lboost_system
#    LIBS += "-L/home/pramod/Dropbox/CDI_PlayGo-Final/Indexer-Minimal/Runtime_Libraries/tbb_android" -ltbb
#}
