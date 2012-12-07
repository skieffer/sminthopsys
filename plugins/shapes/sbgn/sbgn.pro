
QT           += xml svg
TEMPLATE      = lib
CONFIG       += qt plugin
TARGET        = $$qtLibraryTarget(shapes_sbgn)

include(../../../common_options.qmake)
include(../shape_plugin_options.pri)

LIBS         += -L$$DUNNARTBASE/build/plugins -lapplication_sminthopsys

INCLUDEPATH += ../../..
INCLUDEPATH += . $$DUNNARTBASE

SOURCES       = plugin.cpp

