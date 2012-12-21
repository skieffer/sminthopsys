
QT           += xml svg
TEMPLATE      = lib
CONFIG       += qt plugin
TARGET        = $$qtLibraryTarget(fileio_sbml)
LIBS         += -L/usr/local/lib -lsbml

include(../../../common_options.qmake)
include(../fileio_plugin_options.pri)

LIBS        += -Wl,--rpath -Wl,$$DUNNARTBASE/build/plugins
LIBS         += -L$$DUNNARTBASE/build/plugins -lapplication_sminthopsys

DEPENDPATH  += $$DUNNARTBASE/plugins/application/sminthopsys
INCLUDEPATH += $$DUNNARTBASE/plugins/application/sminthopsys

SOURCES       = plugin.cpp

