#----------------------------------------------------------------------
# Pramod Kumar
# Purdue University
#----------------------------------------------------------------------
TEMPLATE = subdirs
SUBDIRS += \
		poly2tri \
		Physics \
		Indexer \
		QsLog \
		Core \
		UI

CONFIG += ordered

SUBDIRS += 	CDI_PlayGo
CDI_PlayGo.absolute_path($$PWD)
