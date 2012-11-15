QT += xml svg
TEMPLATE = lib
TARGET = dsbpe
CONFIG += shared

DEPENDPATH  += .. .
INCLUDEPATH += .. .

include(../common_options.qmake)

# Input
SOURCES += \
        dsbnode.cpp \
        dsbspecies.cpp \
	dsbreaction.cpp \
        dsbcompartment.cpp \
        dsbclone.cpp \
        dsbcell.cpp

HEADERS += \
        dsbnode.h \
        dsbreclayout.h \
        dsbspecies.h \
	dsbreaction.h \
        dsbcompartment.h \
        dsbclone.h \
        dsbcell.h
