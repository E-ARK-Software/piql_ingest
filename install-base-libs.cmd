@echo off

call setup_paths.cmd

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

echo.
echo ************************************
echo *** Installing base libraries... ***
echo ************************************
echo.
echo ************************************
echo *** Installing common library... ***
echo ************************************

cd thirdparty\common\
   qmake -r
   nmake clean
   nmake debug
   nmake release

echo.
echo ************************************
echo ** Installing qt_tools library... **
echo ************************************

   cd ..
   cd qt_tools
   qmake -r
   nmake clean
   nmake debug
   nmake release

echo.
echo ***************************************
echo * Installing service tools library... *
echo ***************************************

   cd ..
   cd service_tools
   qmake -r
   nmake clean
   nmake debug
   nmake release

   cd ..\..

cmd /k
