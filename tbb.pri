unix {
    LIBS += -ltbb
}

win32 {
    INCLUDEPATH += C:/tbb/include/
    Release: LIBS += -LC:/tbb/lib/intel64/vc14/ -ltbb
    Debug: LIBS += -LC:/tbb/lib/intel64/vc14/ -ltbb_debug
}
