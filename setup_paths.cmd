rem Environment variables setup

set CV_ROOT=%CD%
set CV_THIRDPARTY=%CV_ROOT%\..\thirdparty
set CV_QTDIR=C:\Qt\Qt5.12.0\5.12.0\msvc2017_64
set CV_BOOST_INCLUDE=%CV_THIRDPARTY%\boost_1_72_0
set CV_BOOST_LIBS=%CV_THIRDPARTY%\boost_1_72_0\stage\lib
set CV_TCLAP_INCLUDE=%CV_THIRDPARTY%\tclap-1.2.1
set CV_PHPSECLIB_ROOT="%CV_THIRDPARTY%\phpseclib1.0.6"
set CV_PHP_ROOT="%CV_THIRDPARTY%\php"

rem PATH setup

set path=%CV_QTDIR%\bin;%path%
