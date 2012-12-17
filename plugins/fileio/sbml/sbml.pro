
QT           += xml svg
TEMPLATE      = lib
CONFIG       += qt plugin
TARGET        = $$qtLibraryTarget(fileio_sbml)
LIBS         += -L/usr/local/lib -lsbml

DEPENDPATH += ../../.. .
INCLUDEPATH += ../../..

include(../../../common_options.qmake)
include(../fileio_plugin_options.pri)

LIBS         += -L$$DUNNARTBASE/build/plugins -lapplication_sminthopsys
LIBS += -L/home/skieffer/workspace/dunnart/build/plugins -lapplication_sminthopsys

HEADERS       =

SOURCES       = plugin.cpp

