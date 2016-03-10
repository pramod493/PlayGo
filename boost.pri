unix {
    LIBS += -lboost_filesystem -lboost_system
}

win32 {
    INCLUDEPATH += C:/Boost/include/boost-1_60/

    Release: LIBS += -LC:/Boost/lib/ \
	-llibboost_system-vc140-mt-1_60 \
	-llibboost_filesystem-vc140-mt-1_60 \
	-llibboost_program_options-vc140-mt-1_60

    Debug: LIBS += -LC:/Boost/lib/ \
	-llibboost_system-vc140-mt-gd-1_60 \
	-llibboost_filesystem-vc140-mt-gd-1_60 \
	-llibboost_program_options-vc140-mt-gd-1_60

}
