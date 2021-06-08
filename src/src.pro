##############################################################################
#
#   Creation date:  2021-06-08
#   Created by:     Matti Wennberg
#
#   Copyright (c) 2021 Piql AS, Norway. All rights reserved.
#
##############################################################################

!include ($$(CV_ROOT)/piql_ingest.pri) {
    error( "No piql_ingest.pri file found" )
}

TEMPLATE          =   subdirs
CONFIG           +=   ordered

SUBDIRS           =   libs \
                      app
