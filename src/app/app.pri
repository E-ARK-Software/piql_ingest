##############################################################################
#
#   Creation date:  2021-07-17
#   Created by:     Oleksandr Ivanov
#
#   Copyright (c) 2021 Piql AS, Norway. All rights reserved.
#
##############################################################################

# Add the value of the environment variable CV_QTDIR to the visual studio project properties.

# test if windows
win32 {
contains( PLATFORM, W32 ) {
    DEBUG_PLATFORM=Debug|Win32
    RELEASE_PLATFORM=Release|Win32
} else {
    DEBUG_PLATFORM=Debug|x64
    RELEASE_PLATFORM=Release|x64
}

  VCXPROJ_USER_FILE = "$${OUT_PWD}/$${TARGET}.vcxproj.user"
    # generate file contents
    TEMPNAME     = $${QMAKE_QMAKE}     # contains full dir of qmake used
    QTDIR        = $$dirname(TEMPNAME) # gets only the path
    # vcxproj.user template
    VCXPROJ_USER = "<?xml version=\"1.0\" encoding=\"utf-8\"?>$$escape_expand(\\n)\
                        <Project ToolsVersion=\"12.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">$$escape_expand(\\n)\
                            <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='$${DEBUG_PLATFORM}'\">$$escape_expand(\\n)\
                                <LocalDebuggerEnvironment>PATH=$$(CV_QTDIR)/bin;%PATH%$$escape_expand(\\n)\
                                </LocalDebuggerEnvironment>$$escape_expand(\\n)\
                                <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>$$escape_expand(\\n)\
                            </PropertyGroup>$$escape_expand(\\n)\
                            <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='$${RELEASE_PLATFORM}'\">$$escape_expand(\\n)\
                                <LocalDebuggerEnvironment>PATH=$$(CV_QTDIR)/bin;%PATH%$$escape_expand(\\n)\
                                </LocalDebuggerEnvironment>$$escape_expand(\\n)\
                                <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>$$escape_expand(\\n)\
                            </PropertyGroup>$$escape_expand(\\n)\
                        </Project>$$escape_expand(\\n)\
    "
    # write file
    write_file($${VCXPROJ_USER_FILE}, VCXPROJ_USER)  
}
