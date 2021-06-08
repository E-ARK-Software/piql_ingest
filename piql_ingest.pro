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

# Generate translation files
system(lupdate -no-obsolete -verbose piql_ingest.pro)
system(lrelease piql_ingest.pro)
system(lrelease locale/sv_SE/qt_sv_SE.ts)
system(lrelease locale/nb_NO/qt_nb_NO.ts)

QT               +=   xml widgets gui core svg
TEMPLATE          =   app
CONFIG           +=   console qt no_lflags_merge debug_and_release
macx:CONFIG      +=   app_bundle

# Optimize for speed
win32:QMAKE_CXXFLAGS_RELEASE += /O2 /Ob2 /Oi /Ot /Oy

# Enable release with debug:
win32:QMAKE_LFLAGS +=  /DEBUG
win32:QMAKE_LFLAGS +=  /LARGEADDRESSAWARE  

# Supported translations
TRANSLATIONS      = locale/sv_SE/sv_SE.ts \
                    locale/nb_NO/nb_NO.ts

TARGET            =   piql_ingest
OBJECTS_DIR       =   obj/$$PLATFORM/$$CURBUILD
MOC_DIR           =   obj/$$PLATFORM/$$CURBUILD

INCLUDEPATH       =   inc \
                      src/libs/common/inc \
                      src/libs/service_tools/inc \
                      src/libs/qt_tools/inc \
                      $$INCLUDE_TCLAP() \
                      $$INCLUDE_BOOST()

DEPENDPATH       +=   $$INCLUDEPATH

unix:LIBS        +=   lib/$$CURBUILD/libcommon.a \
                      lib/$$CURBUILD/libst_rec_common_base.a \
                      lib/$$CURBUILD/libqtools.a                      

win32:LIBS        =   lib/$$CURBUILD/common.lib \
                      lib/$$CURBUILD/st_rec_common_base.lib \
                      lib/$$CURBUILD/qtools.lib

# Library dependency checking
PRE_TARGETDEPS   +=   $$LIBS
PRE_TARGETDEPS   -=   Ws2_32.lib

LIBS             +=   $$LINK_BOOST()
win32:LIBS       +=   Ws2_32.lib

GUI_SOURCES       =   src/gui/dpiqlingestmainwindow.cpp \
                      src/gui/deditmetadatawindowbase.cpp \
                      src/gui/deditmetadatawindowfile.cpp \
                      src/gui/deditmetadatawindowpackage.cpp \
                      src/gui/dpiqlingestprogressdialog.cpp
GUI_HEADERS       =   src/gui/dpiqlingestmainwindow.h \
                      src/gui/deditmetadatawindowbase.h \
                      src/gui/deditmetadatawindowfile.h \
                      src/gui/deditmetadatawindowpackage.h \
                      src/gui/dpiqlingestprogressdialog.h
FORMS             =   src/gui/dpiqlingestmainwindow.ui \
                      src/gui/deditmetadatawindow.ui

RESOURCES         =   resources.qrc

SOURCES           =   src/piql_ingest.cpp \
                      src/piql_ingest_config.cpp \
                      src/dingestfilelist.cpp \
                      src/dingestfile.cpp \
                      src/dmetadatatemplateitem.cpp \
                      src/dmetadatatemplateitemlist.cpp \
                      src/dmetadatatemplateitemgroup.cpp \
                      src/dmetadatatemplateitemgrouplist.cpp \
                      src/dmetadatatemplate.cpp \
                      src/dmetadataitemlist.cpp \
                      src/dmetadataitem.cpp \
                      src/dmetadataitemgrouplist.cpp \
                      src/dmetadataitemgroup.cpp \
                      src/dphputils.cpp \
                      src/gui/deditmetadatawindowguiitem.cpp \
                      src/gui/deditmetadatawindowtabordermanager.cpp \
                      $$GUI_SOURCES

HEADERS           =   inc/piql_ingest_config.h \
                      inc/dingestfilelist.h \
                      inc/dingestfile.h \
                      inc/dmetadatatemplateitem.h \
                      inc/dmetadatatemplateitemlist.h \
                      inc/dmetadatatemplateitemgroup.h \
                      inc/dmetadatatemplateitemgrouplist.h \
                      inc/dmetadatatemplate.h \
                      inc/dmetadataitemlist.h \
                      inc/dmetadataitem.h \
                      inc/dmetadataitemgrouplist.h \
                      inc/dmetadataitemgroup.h \
                      inc/dphputils.h \
                      src/gui/deditmetadatawindowguiitem.h \
                      src/gui/deditmetadatawindowtabordermanager.h \
                      $$GUI_HEADERS


PHP_FILES.files = $$files( "*.php" )
PHP_FILES.path = Contents/Resources
PHP_FILES_INCLUDES.files = $$files( "includes/*", true )
PHP_FILES_INCLUDES.path = Contents/Resources/includes

CONFIG_FILES.files = piql_ingest.ini
CONFIG_FILES.path = Contents/Resources
QMAKE_BUNDLE_DATA += PHP_FILES CONFIG_FILES PHP_FILES_INCLUDES


macx:ICON=src/gui/resources/icon_32x32.icns
macx:QMAKE_EXTRA_TARGETS += createicons
macx:PRE_TARGETDEPS      += src/gui/resources/icon_32x32.icns
win32:RC_ICONS=src/gui/resources/icon_32x32.ico
