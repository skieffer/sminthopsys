
QT           += xml svg
TEMPLATE      = lib
CONFIG       += qt plugin
TARGET        = $$qtLibraryTarget(shapes_sbgn)

include(../../../common_options.qmake)
include(../shape_plugin_options.pri)

# LIBS        += -Wl,--rpath -Wl,$$DUNNARTBASE/build/plugins

LIBS        += -L
LIBS        += -lapplication_sminthopsys

DEPENDPATH  += $$DUNNARTBASE/plugins/application/sminthopsys
INCLUDEPATH += $$DUNNARTBASE/plugins/application/sminthopsys

SOURCES       = plugin.cpp

