
QT           += xml svg
TEMPLATE      = lib
CONFIG       += qt plugin
TARGET        = $$qtLibraryTarget(application_dsbpe)
LIBS         += -L/usr/local/lib -lsbml

DEPENDPATH += ../../.. .
INCLUDEPATH += ../../..

include(../../../common_options.qmake)
include(../application_plugin_options.pri)

LIBS         += -L$$DUNNARTBASE/build -ldsbpe
LIBS         += -L$$DUNNARTBASE/build/plugins -lshapes_sbgn

HEADERS       = findBranchesDialog.h

SOURCES       = plugin.cpp \
    findBranchesDialog.cpp
