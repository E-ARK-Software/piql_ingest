Installation Instructions

Windows:

1. The project requires an installed application development environment "Visual Studio".
   Only two versions of "Visual Studio" are currently supported: 
   "Visual Studio 2013" (32 bit) and "Visual Studio 2017" (64 bit).

2. The project requires an installed cross-platform QT framework.
   If this framework is not installed, then you need to install it.

   If you use the installer, the components that need to be installed depend on the version of "Visual Studio".
   If you are using "Visual studio 2013" then you need to install QT component "MSVC 2013 32-bit".  
   If you are using "Visual studio 2017" then you need to install QT component "MSVC 2017 64-bit". 

   If you build QT from source, then for this you need to use the corresponding version of the 
   visual studio, which you will later use to work with the project.
   In this case, you must make sure that this QT version supports the work with the required version of the visual studio.
   List of required source packages: qtbase, qtsvg, qttools.
   Official website: https://www.qt.io/

3. The project requires the boost library version 1.72.0. 
   If necessary, you need download and install the latest version of this library.
   Official website: https://www.boost.org/

4. The project requires the TCLAP library version 1.2.2. 
   If necessary, you need download and install the latest version of this library.
   Official website: https://sourceforge.net/projects/tclap/files/

5. It is necessary to unpack two archives php.zip and phpseclib1.0.6.zip into folder "thirdparty".
   This is needed to work with PHP scripts.
   For convenience, libraries boost and TCLAP can also be installed in this folder.

6. It is necessary to download the project sources from the repository 
   https://github.com/piql/piql_ingest.git
   To do this, you can use the git tool (https://git-scm.com/downloads) 
   or the TortoiseGit app (https://tortoisegit.org/).

7. In the project, you need to open the setup_paths.cmd batch file for editing.
   In this file, you need to set values for the following environment variables:
   CV_THIRDPARTY - path to the folder containing unpacked archives php.zip and phpseclib1.0.6.zip.
   QTDIR - path to the QT source. When using the QT installer, this should be the path to the 
           corresponding folder msvc2013 or msvc2017_64, depending on the version of visual studio used.
           If you built QT from the sources, then you need to specify the path to the folder "qtbase".
   CV_BOOST_INCLUDE - the path to the header files of the Boost library
   CV_BOOST_LIBS - the path to the library files of the Boost library
   CV_TCLAP_INCLUDE - the path to the header files of the TCLAP library
   CV_PHPSECLIB_ROOT - the path to folder "phpseclib1.0.6", which we unpacked from archive phpseclib1.0.6.zip.
   CV_PHP_ROOT - the path to folder "php", which we unpacked from archive php.zip.

8. Start the console with the appropriate command file (setup_vs2013.cmd or setup_vs2017_64.cmd).

9. To compile piql_ingest project, in the console that opens, run the following commands:
   qmake -r
   nmake

10.After that, to generate a project for visual studio, you need to run the command:
   qmake -tp vc -r

11.To create an application release, you need to run the command file create-release.cmd with the release version:
   create-release.cmd <version>

Done!

Note: This instruction is valid for the current version of the project, 
      it will be edited as changes are made to the project.
Important: All libraries must be built using the same version of visual studio.