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

TEMPLATE                    =   lib

TARGET                      =   st_rec_common_base

CONFIG                     +=   staticlib
CONFIG                     +=   qt 

##############################################################################

##  DIRECTORIES  ##

INCLUDEPATH                 =   inc \
                                ../common/inc \
                                $$INCLUDE_TCLAP()

##  HEADER FILES  ##
HEADERS                     =   inc/cmd_util_base.h

##  SOURCE FILES  ##
SOURCES                     =   src/cmd_util_base.cpp
