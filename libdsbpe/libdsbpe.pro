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
        dsbcompartment.cpp

HEADERS += \
        dsbspecies.h \
	dsbreaction.h \
        dsbcompartment.h

