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

# Generate translation files
system(lupdate -no-obsolete -verbose app.pro)
system(lrelease -verbose app.pro)
system(lrelease $$(CV_ROOT)/locale/sv_SE/qt_sv_SE.ts)
system(lrelease $$(CV_ROOT)/locale/nb_NO/qt_nb_NO.ts)

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
TRANSLATIONS      =   $$(CV_ROOT)/locale/sv_SE/sv_SE.ts \
                      $$(CV_ROOT)/locale/nb_NO/nb_NO.ts

DESTDIR           =   $$(CV_ROOT)/$$CURBUILD
TARGET            =   piql_ingest
OBJECTS_DIR       =   obj/$$PLATFORM/$$CURBUILD
MOC_DIR           =   obj/$$PLATFORM/$$CURBUILD

INCLUDEPATH       =   appcore \
                      gui \
                      ../libs/common/inc \
                      ../libs/service_tools/inc \
                      ../libs/qt_tools/inc \
                      $$INCLUDE_TCLAP() \
                      $$INCLUDE_BOOST()

DEPENDPATH       +=   $$INCLUDEPATH

unix:LIBS        +=   $$(CV_ROOT)/lib/$$CURBUILD/libcommon.a \
                      $$(CV_ROOT)/lib/$$CURBUILD/libst_rec_common_base.a \
                      $$(CV_ROOT)/lib/$$CURBUILD/libqtools.a                      

win32:LIBS        =   $$(CV_ROOT)/lib/$$CURBUILD/common.lib \
                      $$(CV_ROOT)/lib/$$CURBUILD/st_rec_common_base.lib \
                      $$(CV_ROOT)/lib/$$CURBUILD/qtools.lib

# Library dependency checking
PRE_TARGETDEPS   +=   $$LIBS
PRE_TARGETDEPS   -=   Ws2_32.lib

LIBS             +=   $$LINK_BOOST()
win32:LIBS       +=   Ws2_32.lib

GUI_SOURCES       =   gui/dpiqlingestmainwindow.cpp \
                      gui/deditmetadatawindowbase.cpp \
                      gui/deditmetadatawindowfile.cpp \
                      gui/deditmetadatawindowpackage.cpp \
                      gui/dpiqlingestprogressdialog.cpp \
                      gui/deditmetadatawindowguiitem.cpp \
                      gui/deditmetadatawindowtabordermanager.cpp
GUI_HEADERS       =   gui/dpiqlingestmainwindow.h \
                      gui/deditmetadatawindowbase.h \
                      gui/deditmetadatawindowfile.h \
                      gui/deditmetadatawindowpackage.h \
                      gui/dpiqlingestprogressdialog.h \
                      gui/deditmetadatawindowguiitem.h \
                      gui/deditmetadatawindowtabordermanager.h
FORMS             =   gui/dpiqlingestmainwindow.ui \
                      gui/deditmetadatawindow.ui

RESOURCES         =   resources.qrc

SOURCES           =   appcore/piql_ingest.cpp \
                      appcore/piql_ingest_config.cpp \
                      appcore/dingestfilelist.cpp \
                      appcore/dingestfile.cpp \
                      appcore/dmetadatatemplateitem.cpp \
                      appcore/dmetadatatemplateitemlist.cpp \
                      appcore/dmetadatatemplateitemgroup.cpp \
                      appcore/dmetadatatemplateitemgrouplist.cpp \
                      appcore/dmetadatatemplate.cpp \
                      appcore/dmetadataitemlist.cpp \
                      appcore/dmetadataitem.cpp \
                      appcore/dmetadataitemgrouplist.cpp \
                      appcore/dmetadataitemgroup.cpp \
                      appcore/dphputils.cpp \
                      $$GUI_SOURCES

HEADERS           =   appcore/piql_ingest_config.h \
                      appcore/dingestfilelist.h \
                      appcore/dingestfile.h \
                      appcore/dmetadatatemplateitem.h \
                      appcore/dmetadatatemplateitemlist.h \
                      appcore/dmetadatatemplateitemgroup.h \
                      appcore/dmetadatatemplateitemgrouplist.h \
                      appcore/dmetadatatemplate.h \
                      appcore/dmetadataitemlist.h \
                      appcore/dmetadataitem.h \
                      appcore/dmetadataitemgrouplist.h \
                      appcore/dmetadataitemgroup.h \
                      appcore/dphputils.h \
                      $$GUI_HEADERS

macx:ICON=resources/icon_32x32.icns
macx:QMAKE_EXTRA_TARGETS += createicons
macx:PRE_TARGETDEPS      += resources/icon_32x32.icns
win32:RC_ICONS=resources/icon_32x32.ico

include(add_app_paths.pri)
