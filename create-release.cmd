@if "%1" EQU "" (
    @echo Usage: %0 ^<version^>
    @goto :eof
)

@echo off

call setup_paths.cmd

set appbasename=piql_ingest
set approot="piql_ingest_%1"

rmdir /s /q %approot%
mkdir %approot%
mkdir %approot%\php
mkdir %approot%\thirdparty
mkdir %approot%\includes
mkdir %approot%\locale
xcopy /s /e /y includes\* %approot%\includes
xcopy /s /e /y php\* %approot%\php
xcopy /s /e /y thirdparty\* %approot%\thirdparty
windeployqt release\%appbasename%.exe --dir %approot% --release --compiler-runtime
copy release\%appbasename%.exe %approot%\.
copy resources\windows10\utils\* %approot%\.
copy resources\windows10\dll\* %approot%\.
copy config.php %approot%\.
copy commit-transaction.php %approot%\.
copy cleanup-transaction.php %approot%\.
copy metadata-description.php %approot%\metadata-description.php
copy metadata-description-dublin-core.php %approot%\metadata-description-dublin-core.php
copy metadata-description-package.php %approot%\metadata-description-package.php
copy metadata-description-simple.php %approot%\metadata-description-simple.php
copy release\%app% %approot%\.
copy %appbasename%.ini %approot%\.
xcopy /s /e /y locale\* %approot%\locale

cmd /k
