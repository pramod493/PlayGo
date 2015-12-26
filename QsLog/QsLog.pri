## ------------------------------------------------
## Logging library for qt.
## Use this to capture information during runtime
## Saving file name and line number are optional
## Works on top of qDebug()
## ----------------------------------------------
INCLUDEPATH += $$PWD
#DEFINES += QS_LOG_LINE_NUMBERS
SOURCES += $$PWD/QsLogDest.cpp \
    $$PWD/QsLog.cpp \
    $$PWD/QsDebugOutput.cpp

HEADERS += $$PWD/QSLogDest.h \
    $$PWD/QsLog.h \
    $$PWD/QsDebugOutput.h
