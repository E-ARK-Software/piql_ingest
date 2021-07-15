##############################################################################
#
#   Creation date:  2020-01-08
#   Created by:     Oleksandr Ivanov
#
#   Copyright (c) 2020 Piql AS, Norway. All rights reserved.
#
##############################################################################

!include ($$(CV_ROOT)/src/libs/libs.pri) {
    error( "No libs.pri file found" )
}

TEMPLATE        =   lib
TARGET          =   common

#   Directories
#
INCLUDEPATH     = inc
                              
CONFIG         += staticlib create_prl

win32:QMAKE_CXXFLAGS   += /wd4996

INCLUDEPATH    += $$INCLUDE_BOOST()

### PROJECT INCLUDE FILES

HEADERS     =   inc/dapplication.h \
                inc/dcommon.h \
                inc/dconfig.h \
                inc/derror.h \
                inc/derrorman.h \
                inc/dfilesystemtools.h \
                inc/dpath.h \
                inc/dregexp.h \
                inc/dscriptcontext.h\
                inc/dstringtools.h \
                inc/dsystemtools.h \
                inc/dtclap.h \
                inc/dtimeval.h \
                inc/dtimetools.h \
                inc/dthread.h \
                inc/dtypes.h \
                inc/dversion.h

### PROJECT SOURCE FILES

SOURCES     =   src/dapplication.cpp \
                src/derror.cpp \
                src/derrorman.cpp \
                src/dpath.cpp \
                src/dtimeval.cpp \
                src/dtimetools.cpp \
                src/dstringtools.cpp \
                src/dfilesystemtools.cpp \
                src/dsystemtools.cpp \
                src/dversion.cpp \
                src/dregexp.cpp \
                src/dscriptcontext.cpp
