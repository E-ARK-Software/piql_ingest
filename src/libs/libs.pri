##############################################################################
#
#   Creation date:  2021-06-03
#   Created by:     Matti Wennberg
#
#   Copyright (c) 2021 Piql AS, Norway. All rights reserved.
#
##############################################################################

contains( CURBUILD, debug ) {
    DESTDIR = $$(CV_ROOT)/lib/debug
    OBJECTS_DIR = obj/$$PLATFORM/debug
}

contains( CURBUILD, release ) {
    DESTDIR = $$(CV_ROOT)/lib/release
    OBJECTS_DIR = obj/$$PLATFORM/release
    
    # Optimize for speed
    win32:QMAKE_CXXFLAGS_RELEASE += /O2 /Ob2 /Oi /Ot /Oy
}

## Function to get TCLAP includes
defineReplace(INCLUDE_TCLAP) {
    # INCLUDES is a local variable for this function
    INCLUDES=

    INCLUDES       +=   $$(CV_THIRDPARTY)/tclap-1.2.1/include
    return( $$INCLUDES )
}
