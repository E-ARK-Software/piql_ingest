##############################################################################
#
#   Creation date:  2021-06-03
#   Created by:     Matti Wennberg
#
#   Copyright (c) 2021 Piql AS, Norway. All rights reserved.
#
##############################################################################

CONFIG                     +=   warn_on thread
unix:CONFIG                +=   debug_and_release
win32:CONFIG               +=   windows debug_and_release stl exceptions
# Remove qt default to aviod too many includes/libs in the makefiles.
CONFIG                     -=   qt

# Avoid qmake removing duplicate library names (linux)
CONFIG                     += no_lflags_merge

# Tool used for clean targets, /q is not default
win32:QMAKE_DEL_FILE        =   del /q

# Build all libs with debug information in release mode (and disable optimization)
win32:QMAKE_CXXFLAGS_RELEASE += /Zi
# Ignore missing PDB files for third party libraries
win32:QMAKE_LFLAGS += /ignore:4099

# Don't upgrade the warning "unreferenced formal parameter" to level 3 (SOAP noise)
win32:QMAKE_CXXFLAGS_WARN_ON -= -w34100

TOOLSET                     =   $$(CV_TOOLSET)
contains( TOOLSET, VS2010|VS2012|VS2013|VS2017 ) {
    # Disable warnings from object files with no symbols defined
    QMAKE_LIBFLAGS         +=   /ignore:4221
    DEFINES                +=   _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS
} else {
    QMAKE_CXXFLAGS         +=   -D_FILE_OFFSET_BITS=64 -DBOOST_SYSTEM_NO_DEPRECATED
    macx:QMAKE_LFLAGS      +=   -stdlib=libc++ -liconv -Wno-c++11-narrowing -framework CoreFoundation
    macx:CONFIG            -=   app_bundle
}

##  OS-X PLATFORM ##
macx:OSX_SED_MAGIC="''"


##  LINUX PLATFORM ##
unix {
PLATFORM                    =   LNX
linux-g++-64:PLATFORM       =   LNX64
SYSNAME                     =   $$system(uname -n)
}

##  WIN PLATFORM  ##
win32 {
contains(QT_ARCH, i386) {
    #message("Windows 32-bit")
    PLATFORM              =   W32
} else {
    #message("Windows 64-bit")
    PLATFORM              =   W64
}
}

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        CURBUILD = debug
    }
    CONFIG(release, debug|release) {
        CURBUILD = release
        DEFINES += NDEBUG
    }
}

## Function to get BOOST includes
defineReplace(INCLUDE_BOOST) {
    win32 {
        return ( $$(CV_BOOST_INCLUDE) )
    }

    !win32 {
       _INCLUDE        = $$(CV_BOOST_INCLUDE)
       macx:_INCLUDE  += $$(CV_BOOST_INCLUDE)/boost
       return( $$_INCLUDE )
    }
}

## Function to get BOOST libs
# Must be used after PRE_TARGETDEPS, or it will fail on win32
defineReplace(LINK_BOOST) {
    win32 {
        return ( -L$$(CV_BOOST_LIBS) )
    }

    !win32 {
       _LIBS             = $$(CV_BOOST_LIBS)/libboost_thread.a \
                           $$(CV_BOOST_LIBS)/libboost_date_time.a \
                           $$(CV_BOOST_LIBS)/libboost_regex.a \
                           $$(CV_BOOST_LIBS)/libboost_system.a \
                           $$(CV_BOOST_LIBS)/libboost_chrono.a
       unix:!macx:_LIBS += -lrt
       return( $$_LIBS )
   }
}

## Function to get TCLAP includes
defineReplace(INCLUDE_TCLAP) {
    # INCLUDES is a local variable for this function
    INCLUDES=

    INCLUDES       +=   $$(CV_THIRDPARTY)/tclap-1.2.1/include
    return( $$INCLUDES )
}
