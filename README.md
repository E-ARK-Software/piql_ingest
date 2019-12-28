Installation Instructions

Windows:
1. The project requires an installed cross-platform QT framework (version 5.1.1). 
   If this framework is not installed, then you need to install it. 
   Official website: https://www.qt.io/
2. The project requires the boost library version 1.72.0. 
   If necessary, you need download and install the latest version of this library. 
   Official website: https://www.boost.org/
3. It is necessary to download the project sources from the repository 
   https://github.com/piql/piql_ingest.git
   To do this, you can use the git tool (https://git-scm.com/downloads) 
   or the TortoiseGit app (https://tortoisegit.org/).
4. The project requires an installed application development environment "Visual Studio".
   Only two versions of "Visual Studio" are currently supported: 
   "Visual Studio 2013" (32 bit) and "Visual Studio 2017" (64 bit).
   In the project, you need to open the appropriate batch file for editing, 
   depending on the version of the installed visual studio: setup_vs2013.cmd or setup_vs2017_64.cmd
   In this file, you need to set values for the following environment variables:
   QTDIR - path to the QT source
   CV_BOOST_INCLUDE - the path to the header files of the Boost library
   CV_BOOST_LIBS - the path to the library files of the Boost library
5. Start the console with the appropriate command file (setup_vs2013.cmd or setup_vs2017_64.cmd).
6. In the console that opens, run the following commands:
   qmake -r
   nmake
7. After that, to generate a project for visual studio, you need to run the command:
   qmake -tp vc -r

Done!

Note: This instruction is valid for the current version of the project, 
      it will be edited as changes are made to the project.