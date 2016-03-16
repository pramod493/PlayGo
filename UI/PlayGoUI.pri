INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/tabletapplication.cpp \
	$$PWD/cdiwindow.cpp \
	$$PWD/cdisearchgraphicsitem.cpp \
	$$PWD/sketchview.cpp \
	$$PWD/colorselectortoolbar.cpp \
	$$PWD/modelviewtreewidget.cpp \
	$$PWD/drawplotui.cpp

HEADERS += \
	$$PWD/tabletapplication.h \
	$$PWD/cdiwindow.h \
	$$PWD/uisettings.h \
	$$PWD/cdisearchgraphicsitem.h \
	$$PWD/sketchview.h \
	$$PWD/colorselectortoolbar.h \
	$$PWD/modelviewtreewidget.h \
	$$PWD/drawplotui.h

# include plot library
CDI_PLOT_LIB_PATH=$$PWD/../qCustomPlot
INCLUDEPATH += $${CDI_PLOT_LIB_PATH}
SOURCES += $${CDI_PLOT_LIB_PATH}/qcustomplot.cpp 
HEADERS += $${CDI_PLOT_LIB_PATH}/qcustomplot.h

FORMS += \
	$$PWD/drawplotui.ui
