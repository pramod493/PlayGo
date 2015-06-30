unix {
    #-------------------------------------------------------------
    #       LINUX SPECIFIC OPTIONS
    LIBS += -lopencv_core -lopencv_features2d -lopencv_highgui -lopencv_shape -lopencv_ts -lopencv_imgproc -lopencv_photo -lopencv_imgcodecs
    #LIBS += -lopencv_core -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc  -lopencv_nonfree -lopencv_photo
    LIBS += -lboost_filesystem -lboost_system
    LIBS += -ltbb
}

win32 {
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
