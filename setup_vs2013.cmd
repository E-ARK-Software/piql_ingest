@echo off

set QTDIR=c:\Qt\5.5\Src\qtbase\
set CV_BOOST_INCLUDE="..\..\cmu_112\thirdparty\w32\boost_1_60_0"
set CV_BOOST_LIBS="..\..\cmu_112\thirdparty\w32\boost_1_60_0\stage\lib"

rem PATH setup

set path=%QTDIR%\bin;%path%
set path=%path%;%CYGWIN%

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

cmd /k
