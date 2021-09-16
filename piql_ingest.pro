##############################################################################
#
#   Creation date:  2019-12-04
#   Created by:     Oleksandr Ivanov
#
#   Copyright (c) 2019 Piql AS, Norway. All rights reserved.
#
##############################################################################

!include ($$(CV_ROOT)/piql_ingest.pri) {
    error( "No piql_ingest.pri file found" )
}

# Get php resources
##  LINUX PLATFORM ##
unix {
    system(mkdir -p thirdparty)
    system(mkdir -p thirdparty/phpseclib)
    system(cp -r $$(CV_PHPSECLIB_ROOT)/* thirdparty/phpseclib/.)
    system(mkdir -p thirdparty/metsgen)
    system(unzip $$(CV_METSGEN_ROOT)/metsgen_2.0.1.zip -d thirdparty/metsgen/.)
}
##  WIN PLATFORM  ##
win32 {
    system(mkdir thirdparty\phpseclib)
    system(xcopy /d /s /e /y %CV_PHPSECLIB_ROOT%\* thirdparty\phpseclib)
    system(WinRAR x -y data/util/metsgen.zip thirdparty)

    system(mkdir php)
    system(xcopy /d /s /e /y %CV_PHP_ROOT%\* php)
}

TEMPLATE          =   subdirs
CONFIG           +=   ordered

SUBDIRS           =   src

PHP_FILES.files = $$files( "*.php" )
PHP_FILES.path = Contents/Resources
PHP_FILES_INCLUDES.files = $$files( "includes/*", true )
PHP_FILES_INCLUDES.path = Contents/Resources/includes

CONFIG_FILES.files = piql_ingest.ini
CONFIG_FILES.path = Contents/Resources
QMAKE_BUNDLE_DATA += PHP_FILES CONFIG_FILES PHP_FILES_INCLUDES
