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


Updating translation files for PHP scripts in Windows.

1. You need to install tool Cygwin. https://cygwin.com/install.html
2. For Cygwin, install packages "gettext" and "gettext_devel" required to work with translation files.
3. Launch Windows console. To do this, you can just use batch file "setup_vs2013.cmd" or "setup_vs2017_64.cmd".
4. In the Windows console you need to add the path to folder "cygwin64\bin" to environment variable "PATH".
   For example by running the following command: set path=C:\cygwin64\bin;%path%
5. You need to go to the folder containing the translation files for the PHP scripts. 
   This is the folder containing two files "messages.po" and "messages.mo".
   To do this, run the following command in the console (this is an example for Norwegian):
   cd <path to the project folder>\locale\nb_NO\LC_MESSAGES\
6. Save a copy of the old translation file. To do this, run the following command in an open console:
   mv messages.po old.po
7. Create a blank translation file from source code. To do this, run the following command in the console:
   find ./../../../ -iname "*.php" -not -path "*piql_ingest*" | xargs xgettext -n
8. Merge the old translation file with the blank one. To do this, run the following command in the console:
   msgmerge old.po messages.po --output-file=new.po
9. Modify the file new.po using a gettext editor or a text editor (make sure you save as UTF-8).
10.Rename the final output to messages.po:
   mv new.po messages.po
11.Compile the file into machine readable (messages.mo):
   msgfmt messages.po
   
Done!


Note: This instruction is valid for the current version of the project, 
      it will be edited as changes are made to the project.
Important: All libraries must be built using the same version of visual studio.