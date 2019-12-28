@echo off

set QTDIR=thirdparty\QT_5_5_1\qtbase-5.5.1
set CV_BOOST_INCLUDE="thirdparty\boost_1_72_0"
set CV_BOOST_LIBS="thirdparty\boost_1_72_0\stage\lib"

rem PATH setup

set path=%QTDIR%\bin;%path%

pushd "%VS120COMNTOOLS%"\..\..\vc\
call vcvarsall.bat"" x86
popd
if ERRORLEVEL 1 (
echo.
echo ERROR: Visual Studio 2013 is not installed!
echo.  
goto :EOF
)

rem Setup Qt to use this compiler toolset
set QMAKESPEC=win32-msvc2013

echo =========================================================================
echo Setup the Win32 dev environment - Visual Studio 2013
echo =========================================================================
echo.
echo QTDIR=%QTDIR%
echo QMAKESPEC=%QMAKESPEC%
echo CV_BOOST_INCLUDE=%CV_BOOST_INCLUDE%
echo CV_BOOST_LIBS=%CV_BOOST_LIBS%
echo =========================================================================
echo.

echo =========================================================================
echo Install additional tools
echo =========================================================================
echo.

call install-tools.cmd

cmd /k
