unix {
    LIBS += -lopencv_core -lopencv_features2d -lopencv_shape -lopencv_ts -lopencv_imgproc -lopencv_photo -lopencv_imgcodecs
}

win32 {
    INCLUDEPATH += C:/opencv/build/include/
    Release: LIBS += -LC:/opencv/build/x64/vc14/lib -lopencv_world310s
    Debug: LIBS += "-LC:/opencv/build/x64/vc14/lib" -lopencv_world310d
}
