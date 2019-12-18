@echo off

set QTDIR=C:\Qt\5.12.2\msvc2017_64
set CV_BOOST_INCLUDE="..\..\cmu_112\thirdparty\w32\boost_1_60_0"
set CV_BOOST_LIBS="..\..\cmu_112\thirdparty\w32\boost_1_60_0\stage\lib"

rem PATH setup
set path=%QTDIR%\bin;%path%

pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
call vcvarsall.bat"" x64
popd
if ERRORLEVEL 1 (
echo.
echo ERROR: Visual Studio 2017 is not installed!
echo.  
goto :EOF
)

rem Setup Qt to use this compiler toolset
set QMAKESPEC=win32-msvc

echo =========================================================================
echo Setup the Win64 dev environment - Visual Studio 2017
echo =========================================================================
echo.
echo QTDIR=%QTDIR%
echo QMAKESPEC=%QMAKESPEC%
echo CV_BOOST_INCLUDE=%CV_BOOST_INCLUDE%
echo CV_BOOST_LIBS=%CV_BOOST_LIBS%
echo =========================================================================
echo.

cmd /k
