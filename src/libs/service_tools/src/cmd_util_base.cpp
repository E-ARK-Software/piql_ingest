/*****************************************************************************
**  $Id$
**
**  Base class for command line utils
**
**  Creation date:  2010-01-08
**  Created by:     Ole Liabo
**
**  Platform(s):    Intel, Linux 2.4.x, g++ 3.0
**                  Intel, Windows 2K/XP, VC++ 6.0
**
**  Dependencies:   -
**  Restrictions:   -
**  ToDo:           -
**
**  Copyright (c) 2010 Piql. All rights reserved.
**
**  This file is part of the service tools common library.
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "cmd_util_base.h"

//  SYSTEM INCLUDES
//
#include <iostream>
#include "dtclap.h"

//  QT INCLUDES
//
#include <QStandardPaths>

//  CV INCLUDES
//
#include "dcommon.h"
#include "derrorman.h"
#include "derror.h"
#include "dversion.h"
#include "dpath.h"

//  NAMESPACE
//
D_NAMESPACE_USING(D_NAMESPACE)
D_NAMESPACE_USING(std)
D_NAMESPACE_USING(TCLAP)

//----------------------------------------------------------------------------
/*! Constructor
 */

CmdUtilBase::CmdUtilBase(
    const std::string& applicationName, 
    const DVersion& version, 
    const std::string& helpText)
    : DApplication( D_APP_DAEMON, applicationName.c_str(), version, applicationName.c_str() ),
      m_helpText(helpText),
      m_applicationName(applicationName),
      m_version(version),
      m_isLogLevelSet(false)
{
    // Create the command line parameters object
    m_cmdLine = new CmdUtilParameters( m_helpText, ' ', m_version.asString(), true );

    // We should handle the exceptions from tclap
    m_cmdLine->setExceptionHandling( false );
}

//----------------------------------------------------------------------------
/*! Destructor
 */

CmdUtilBase::~CmdUtilBase()
{
    delete m_cmdLine;
}

//----------------------------------------------------------------------------
/*! The 'main' function. 
 *
 *  Parses the command line and passes control to to util.
 */

int CmdUtilBase::launch( int argc, char *argv[] )
{
    m_argc = argc;
    m_argv = argv;

    return run();
}

int CmdUtilBase::execute()
{
    int argc = m_argc;
    char **argv = m_argv;

    string defaultLog;

    DErrorMan::Instance()->setModuleName( string(applicationName()) );
#if defined ( D_OS_WIN32 )
    DPath logPath( QStandardPaths::writableLocation(QStandardPaths::TempLocation).toStdString(), string(applicationName()) + ".log" );
    defaultLog = logPath.path();
    //DErrorMan::Instance()->setLogStream( string(applicationName()) + ".log");
#elif defined ( D_OS_X )
    defaultLog = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ).toStdString()
      + "/" + applicationName() + ".log";
#else
    defaultLog = "/usr/tmp/" + applicationName() + ".log";
    //DErrorMan::Instance()->setLogStream("/usr/tmp/" + applicationName() + ".log");
#endif
    DErrorMan::Instance()->setLogEnabled( true );
    // Setting to INFO here, applications can override later
    DErrorMan::Instance()->setLogLevel( DError::TYPE_INFO );
    ERROR_F( "CmdUtilBase::launch" );
    error << ERRinfo << "Session started: " << applicationName() << " version: " << version() << endl;

    // Command line handling
    int result = 1;
    try 
    {
        // Add parameters common for all utils
        ValueArg<string> logFile("", "log", "Log file, default: " + defaultLog, false, defaultLog, "log file");
        if ( !addArgument( logFile ) )
        {
            return 1;
        }

        ValueArg<string> logLevel( "", "log-level", "Log level: DEBUG, INFO, WARNING, ERROR or FATAL, default: ERROR", false, "ERROR", "log level" );
        if ( !addArgument( logLevel ) )
        {
            return 1;
        }

        // Parse the args.
        m_cmdLine->parse( argc, argv );

        m_isLogLevelSet = logLevel.isSet();

        // Setup logging
        if( logFile.getValue() != "STDOUT" )
        {
            DErrorMan::Instance()->setLogStream( logFile.getValue() );
        }
        DErrorMan::Instance()->setLogEnabled( true );

        if ( logLevel.getValue() == "DEBUG" )
        {
            DErrorMan::Instance()->setLogLevel(DError::TYPE_DEBUG);
        }
        else if ( logLevel.getValue() == "INFO" )
        {
            DErrorMan::Instance()->setLogLevel(DError::TYPE_INFO);
        }
        else if ( logLevel.getValue() == "WARNING" )
        {
            DErrorMan::Instance()->setLogLevel(DError::TYPE_WARNING);
        }
        else if ( logLevel.getValue() == "ERROR" )
        {
            DErrorMan::Instance()->setLogLevel(DError::TYPE_ERROR);
        }
        else if ( logLevel.getValue() == "FATAL" )
        {
            DErrorMan::Instance()->setLogLevel(DError::TYPE_FATAL);
        }
        else
        {
            cerr << "Illegal log level: " << logLevel.getValue() << endl;
            return 1;
        }

        // Call tool spesific argument handling, this the result could be:
        //  a) The tool want to exit
        //  b) The tool want to contunue processing
        if ( !parseArguments(*m_cmdLine) )
        {
            return 1;
        }

        // Let the tool do the rest..
        result = launchHandler();
    }
    catch (ExitException &/*e*/)
    {
        result = 0;
    }
    catch (ArgException &e)  // catch any exceptions
    {
        cerr << "error: " << e.error() << " for arg " << e.argId() << endl; 
    }

    return result;
}

//----------------------------------------------------------------------------
/*! Return util help text.
 *
 */

const std::string& CmdUtilBase::helpText() const
{
    return m_helpText;
}

//----------------------------------------------------------------------------
/*! Return the application name.
 *
 */

const std::string& CmdUtilBase::applicationName() const
{
    return m_applicationName;
}

//----------------------------------------------------------------------------
/*! Return the application version.
 *
 */

const DVersion& CmdUtilBase::version() const
{
    return m_version;
}

//----------------------------------------------------------------------------
/*! Add a command line argument.
 *
 */

bool CmdUtilBase::addArgument( CmdUtilArgument& argument )
{
    m_cmdLine->add( argument );
    return true;
}

//----------------------------------------------------------------------------
/*! Add mutual exclusive command line arguments.
 *
 */

bool CmdUtilBase::addExclusiveArguments( CmdUtilArgument& a, CmdUtilArgument& b )
{
    m_cmdLine->xorAdd( a, b );
    return true;
}

bool CmdUtilBase::addExclusiveArguments( CmdUtilArgument& a, CmdUtilArgument& b, CmdUtilArgument& c )
{
    vector<Arg*> xorList;
    xorList.push_back( &a );
    xorList.push_back( &b );
    xorList.push_back( &c );

    m_cmdLine->xorAdd( xorList );
    return true;
}

bool CmdUtilBase::addExclusiveArguments( CmdUtilArgument& a, CmdUtilArgument& b, CmdUtilArgument& c, CmdUtilArgument& d )
{
    vector<Arg*> xorList;
    xorList.push_back( &a );
    xorList.push_back( &b );
    xorList.push_back( &c );
    xorList.push_back( &d );

    m_cmdLine->xorAdd(xorList);
    return true;
}

bool CmdUtilBase::addExclusiveArguments( CmdUtilArguments& arguments )
{
    m_cmdLine->xorAdd( arguments );
    return true;
}

bool CmdUtilBase::isLogLevelSet() const
{
    return m_isLogLevelSet;
}
  
