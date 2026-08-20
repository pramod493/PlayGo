## Common for all libs
## do not call before setting target
CONFIG += c++17
QMAKE_CXXFLAGS_WARN_OFF += -Wunused-parameter

unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += opencv4

	CONDA_PREFIX = $$(CONDA_PREFIX)
	!isEmpty(CONDA_PREFIX) {
		INCLUDEPATH += $$CONDA_PREFIX/include
		LIBS += -L$$CONDA_PREFIX/lib
		QMAKE_RPATHDIR += $$CONDA_PREFIX/lib
	}
}

CONFIG(debug, debug|release): TARGET=$${TARGET}_debug
DESTDIR = $$PWD/lib/
