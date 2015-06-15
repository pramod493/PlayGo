#------------------------------------------------------------
#       WINDOWS SPECIFIC OPTIONS
INCLUDEPATH += $$PWD/boost_1_58_0/
INCLUDEPATH += $$PWD/tbb/include/
INCLUDEPATH += $$PWD/opencv/build/include/

release
{
    # Note - These are only for release mode only. Add If/Else for release/debug mode
    LIBS += "-L$$PWD/boost_1_58_0/stage/lib/x64" -llibboost_system-vc120-mt-1_58 -llibboost_filesystem-vc120-mt-1_58 -llibboost_program_options-vc120-mt-1_58
    LIBS += "-L$$PWD/tbb/lib/intel64/vc12/" -ltbb
    LIBS += "-L$$PWD/opencv/build/x64/vc12/lib" -lopencv_ts300 -lopencv_world300
    #------------------------------------------------------------
}
#debug {
#    # Note - These are only for release mode only. Add If/Else for release/debug mode
#    LIBS += "-L$$PWD/boost_1_58_0/stage/lib/x64" -llibboost_system-vc120-mt-gd-1_58 -llibboost_filesystem-vc120-mt-gd-1_58 -llibboost_program_options-vc120-mt-gd-1_58
#    LIBS += "-L$$PWD/tbb/lib/intel64/vc12/" -ltbb_debug
#    LIBS += "-L$$PWD/opencv/build/x64/vc12/lib" -lopencv_ts300d -lopencv_world300d
#}
