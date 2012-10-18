
QT           += xml svg
TEMPLATE      = lib
CONFIG       += qt plugin
TARGET        = $$qtLibraryTarget(application_dsbpe)

include(../../../common_options.qmake)
include(../application_plugin_options.pri)

HEADERS       =
SOURCES       = plugin.cpp
