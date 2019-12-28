@echo off

set PHPSECLIB_DESTINATION=includes\thirdparty\phpseclib1.0.6
set PHP_DESTINATION=php
set TCLAP_DESTINATION=thirdparty\tclap-1.2.1

set path=resources\windows10\utils;%path%

echo Installing phpseclib1.0.6

if not exist "%PHPSECLIB_DESTINATION%" (
md %PHPSECLIB_DESTINATION%
)

WinRAR x -y thirdparty/phpseclib1.0.6.zip %PHPSECLIB_DESTINATION%

echo Installing PHP

if not exist "%PHP_DESTINATION%" (
md %PHP_DESTINATION%
)

WinRAR x -y thirdparty/php.zip %PHP_DESTINATION%

echo Installing tclap-1.2.1

if not exist "%TCLAP_DESTINATION%" (
md %TCLAP_DESTINATION%
)

WinRAR x -y thirdparty/tclap-1.2.1.zip %TCLAP_DESTINATION%
