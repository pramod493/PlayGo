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

#opencv
unix {
	LIBS += -lopencv_core -lopencv_features2d -lopencv_shape -lopencv_ts -lopencv_imgproc -lopencv_photo -lopencv_imgcodecs
}
win32 {
	INCLUDEPATH += C:/opencv/build/include/
        Release: LIBS += -LC:/opencv/build/x64/vc14/lib -lopencv_world320
        Debug: LIBS += "-LC:/opencv/build/x64/vc14/lib" -lopencv_world320d
}

#tbb
unix {
	LIBS += -ltbb
}
win32 {
	INCLUDEPATH += C:/tbb/include/
	Release: LIBS += -LC:/tbb/lib/intel64/vc14/ -ltbb
	Debug: LIBS += -LC:/tbb/lib/intel64/vc14/ -ltbb_debug
}

#NOTE - This might not work on linux where libs are named differently
INCLUDEPATH += $$PWD/Indexer
INCLUDEPATH += $$PWD/poly2tri
INCLUDEPATH += $$PWD/Physics
INCLUDEPATH += $$PWD/QsLog
INCLUDEPATH += $$PWD/P-Dollar-Recognizer
INCLUDEPATH += $$PWD/qCustomPlot
INCLUDEPATH += $$PWD/EdgeDetect
INCLUDEPATH += $$PWD/Core
INCLUDEPATH += $$PWD/UI
INCLUDEPATH += $$PWD/Controller

#NOTE - This might not work on linux where libs are named differently
CONFIG(debug, debug|release): LIBS += -L$$PWD/lib \
			-lpoly2tri_debug \
			-lbox2d_debug \
			-lindexer_debug \
			-lplaygocore_debug \
			-lplaygoui_debug \
			-lqslog_debug \
			-lpdollar_debug
CONFIG(release, debug|release): LIBS += -L$$PWD/lib \
			-lpoly2tri \
			-lbox2d \
			-lindexer \
			-lplaygocore \
			-lplaygoui \
			-lqslog \
			-lpdollar
