
QT           += xml svg
TEMPLATE      = lib
CONFIG       += qt plugin
TARGET        = $$qtLibraryTarget(application_sminthopsys)
LIBS         += -L/usr/local/lib -lsbml

DEPENDPATH += ../../.. .
INCLUDEPATH += ../../..

include(../../../common_options.qmake)
include(../application_plugin_options.pri)

#LIBS         += -L$$DUNNARTBASE/build/plugins -lshapes_sbgn

HEADERS += findBranchesDialog.h \
           freepathway.h \
           dsbpathway.h \
           dsbbranch.h \
           dsbfork.h \
           dsbnode.h \
           dsbreclayout.h \
           dsbspecies.h \
           dsbreaction.h \
           dsbcompartment.h \
           dsbclone.h \
           pdepn.h \
           pdunspecifiedepn.h \
    pdsourcesink.h \
    pdsimplechemepn.h \
    pdmacromolepn.h \
    pdnucleicepn.h \
    pdcomplexepn.h \
    pdperturbingepn.h \
    pdphenotypeprocessnode.h \
    pdlogicaloperators.h \
    pdprocessnode.h

SOURCES += plugin.cpp \
           findBranchesDialog.cpp \
           freepathway.cpp \
           dsbpathway.cpp \
           dsbbranch.cpp \
           dsbfork.cpp \
           dsbnode.cpp \
           dsbspecies.cpp \
           dsbreaction.cpp \
           dsbcompartment.cpp \
           dsbclone.cpp \
    pdepn.cpp \
    pdunspecifiedepn.cpp \
    pdsourcesink.cpp \
    pdsimplechemepn.cpp \
    pdmacromolepn.cpp \
    pdnucleicepn.cpp \
    pdcomplexepn.cpp \
    pdperturbingepn.cpp \
    pdphenotypeprocessnode.cpp \
    pdlogicaloperators.cpp \
    pdprocessnode.cpp

