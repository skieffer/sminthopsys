QT += xml svg
TEMPLATE = lib
TARGET = dsbpe
CONFIG += shared

DEPENDPATH  += .. .
INCLUDEPATH += .. .

include(../common_options.qmake)

# Input
SOURCES += \
        freepathway.cpp \
        dsbpathway.cpp \
        dsbbranch.cpp \
        dsbfork.cpp \
        dsbnode.cpp \
        dsbspecies.cpp \
	dsbreaction.cpp \
        dsbcompartment.cpp \
        dsbclone.cpp

HEADERS += \
        freepathway.h \
        dsbpathway.h \
        dsbbranch.h \
        dsbfork.h \
        dsbnode.h \
        dsbreclayout.h \
        dsbspecies.h \
	dsbreaction.h \
        dsbcompartment.h \
        dsbclone.h
