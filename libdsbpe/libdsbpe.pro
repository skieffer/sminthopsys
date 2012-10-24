TEMPLATE = lib
TARGET = dsbpe
CONFIG += shared

DEPENDPATH  += .. .
INCLUDEPATH += .. .

include(../common_options.qmake)

# Input
SOURCES += \
        dsbspecies.cpp \
	dsbreaction.cpp \
        dsbabstractcontainer.cpp

HEADERS += \
        dsbspecies.h \
	dsbreaction.h \
        dsbabstractcontainer.h

