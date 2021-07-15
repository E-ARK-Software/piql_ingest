@echo off

call setup_paths.cmd

for /f "usebackq delims=" %%a in (`"%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere" -latest -property installationPath`) do set VSPATH=%%a

pushd "%VSPATH%"\vc\Auxiliary\Build\
if not defined DevEnvDir (
call vcvarsall.bat"" x64
)
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
echo CV_QTDIR=%CV_QTDIR%
echo QMAKESPEC=%QMAKESPEC%
echo CV_BOOST_INCLUDE=%CV_BOOST_INCLUDE%
echo CV_BOOST_LIBS=%CV_BOOST_LIBS%
echo =========================================================================
echo.

cmd /k
