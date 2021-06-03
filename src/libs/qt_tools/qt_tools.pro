##############################################################################
#
#   Creation date:  2020-01-09
#   Created by:     Oleksandr Ivanov
#
#   Copyright (c) 2020 Piql AS, Norway. All rights reserved.
#
##############################################################################

!include ($$(CV_ROOT)/src/libs/libs.pri) {
    error( "No libs.pri file found" )
}

TEMPLATE            =   lib
TARGET              =   qtools
QT                 +=   widgets

#   Directories
#
INCLUDEPATH    =   inc \
                   ../common/inc

CONFIG         += staticlib qt create_prl

### COMPILER FLAGS

win32:QMAKE_LFLAGS     += /NODEFAULTLIB:"libc" \
                          /NODEFAULTLIB:"libc.lib" \
                          /NODEFAULTLIB:"libcmt.lib" \
                          /NODEFAULTLIB:"libcd.lib" \
                          /NODEFAULTLIB:"libcmtd.lib " \
                          /NODEFAULTLIB:"msvcrtd.lib" 

### PROJECT INCLUDE FILES

HEADERS     =   inc/qpersistantfiledialog.h \
                inc/dqterror.h

### PROJECT SOURCE FILES

SOURCES     =   src/qpersistantfiledialog.cpp \
                src/dqterror.cpp
