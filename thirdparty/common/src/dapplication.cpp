/***************************************************************************
**  $Id: dapplication.cpp,v 1.1 2005/05/16 12:46:08 swhbt Exp $
**
**  Implementation of the DLFR DApplication application class
**
**  Creation date:  2004/05/25
**  Created by:     Han Timmerman
**
**  Project ID:     NO-04001-01S
**  Project name:   Piql DLFR
**  Project status: Create
**
**  Target(s):      Linux 2.4.x / GCC 3.0
**
**  Namespace:      DLFR
**
**  Dependencies:   -
**  Restrictions:   -
**  ToDo:           -
**
**  Copyright (c) 2004 ANT Software Development. All rights reserved.
**
**  This file is part of the DLFR common library
**
***************************************************************************/

//  PROJECT INCLUDES
//
#include    "dapplication.h"
#include    "derrorman.h"
#include    "derror.h"
#include    "dfilesystemtools.h"
#include    "dpath.h"

//  SYSTEM INCLUDES
//
#include    <iostream>

#include    <sys/types.h>
#include    <sys/stat.h>
#if defined ( D_OS_UNIX )
#   include <string.h>  
#   include <unistd.h>
#   include <stdlib.h>
#elif defined ( D_OS_WIN32 )
#   include <io.h>
#   include <process.h>
#endif // D_OS_UNIX
#include    <fcntl.h>
#include    <stdio.h>
#include    <errno.h>

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )

//  STATIC INITIALIZATION
//
DApplication*  D_NAMESPACE::cvApp = 0;


/****************************************************************************/
/*! \class  DApplication  dapplication.h
 *  \ingroup basecommon
 *  \brief  This class represents a DLFR application.
 *
 *  This class represents a DLFR application. That can be a server program or
 *  a GUI program.
 */


//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     DApplication::DApplication( int type, const char* argv0, const char* name )
 *  \param  type    Application type (D_APP_xxxx)
 *  \param  argv0   Command line option argv[0]
 *  \param  name    Application name
 *
 *  Create an application object.
 */

DApplication::DApplication( int type, const char* argv0, const char* name )
{
    if (cvApp != 0)
    {
        cerr << "Application duplication prohibited!" << endl;
        exit( 1 );
    }

    cvApp = (DApplication*) this;

    DPath  path;

    if (argv0 != 0)
    {
        path.setPath( argv0 );
    }

    m_AppType = type;
    m_AppDirectory = path.directory();
    m_AppName = (name != 0) ? name : path.basename();
    m_ErrorManager = DErrorMan::Instance();
    m_ErrorManager->setModuleName( m_AppName );
}


//----------------------------------------------------------------------------
/*! \fn     DApplication::DApplication( int type, const char* argv0, const DVersion& ver, const char* name )
 *  \param  type    Application type (D_APP_xxxx)
 *  \param  argv0   Command line option argv[0]
 *  \param  ver     Application release version
 *  \param  name    Application name
 *
 *  Create an application object.
 */

DApplication::DApplication( int type, const char* argv0, const DVersion& ver, const char* name )
{
    if (cvApp != 0)
    {
        cerr << "Application duplication prohibited!" << endl;
        exit( 1 );
    }

    cvApp = (DApplication*) this;

    DPath  path;

    if (argv0 != 0)
    {
        path.setPath( argv0 );
    }

    m_AppType = type;
    m_AppDirectory = path.directory();
    m_AppName = (name != 0) ? name : path.basename();
    m_AppVersion = ver;
    m_ErrorManager = DErrorMan::Instance();
    m_ErrorManager->setModuleName( m_AppName );
}


//----------------------------------------------------------------------------
/*! \fn     DApplication::~DApplication()
 *
 *  Destructor.
 */

DApplication::~DApplication()
{
}


//----------------------------------------------------------------------------
/*! \fn     const string&  DApplication::name() const
 *  \return reference to application name string
 *
 *  Return reference to application name string.
 */


//----------------------------------------------------------------------------
/*! \fn     const string&  DApplication::directory() const
 *  \return reference to directory string
 *
 *  Return reference to application directory string.
 */


//----------------------------------------------------------------------------
/*! \fn     void  DApplication::setVersion( const DVersion& ver )
 *  \param  ver     Version info
 *
 *  Construct application release string.
 */

void  DApplication::setVersion( const DVersion& ver )
{
    m_AppVersion = ver;
}


//----------------------------------------------------------------------------
/*! \fn     const DVersion&  DApplication::release() const
 *  \return application release
 *
 *  Return application release.
 */


//----------------------------------------------------------------------------
/*! \fn     string  DApplication::releaseInfo() const
 *  \return application release string
 *
 *  Return application release string.
 */

string  DApplication::releaseInfo() const
{
    string  ret = "V" + m_AppVersion.asString( true );

    return  ret;
}


//----------------------------------------------------------------------------
/*! 
 *  \param  path    Configuration file path
 *  \param  check   Check for existing file flag
 *  \return success state
 *
 *  Set configuration file path. If no path is specified, the path will be
 *  created from the application name and base directory.
 */

bool  DApplication::setConfigPath( const std::string& path, bool check )
{
    if (path.empty())
    {
        m_ConfigPath = directory() + "/" + name() + ".conf";
    }
    else
    {
        m_ConfigPath = path;
    }

    if (check)
    {
        return  (DFileSystemTools::isExistingPath( m_ConfigPath ) && DFileSystemTools::isReadablePath( m_ConfigPath ));
    }

    return  true;
}


//----------------------------------------------------------------------------
/*! \fn     const string&  DApplication::configPath() const
 *  \return configuration file path
 *
 *  Return configuration file path.
 */


//----------------------------------------------------------------------------
/*! 
 *  \param  path    Configuration file path
 *  \param  renew   Create new log file flag
 *  \return success state
 *
 *  Set log file path. If no path is specified, the path will be
 *  created from the application name and base directory.
 *  If the \a renew parameter is specified as \a true, an existing log file
 *  will be renamed and a new log file will be created.
 */

bool  DApplication::setLogPath( const std::string& path, bool renew )
{
    if (path.empty())
    {
        m_LogPath = directory() + "/" + name() + ".log";
    }
    else
    {
        m_LogPath = path;
    }

    if ((renew) && DFileSystemTools::isExistingPath( m_LogPath ))
    {
        //  TODO: Rename old log file
    }

    // Check if log dir exists
    DPath logDir(m_LogPath);
    if (!DFileSystemTools::isExistingPath(logDir.directory()))
    {
        return false;
    }

    return  m_ErrorManager->setLogStream( m_LogPath );
}


//----------------------------------------------------------------------------
/*! \fn     const string&  DApplication::logPath() const
 *  \return log file path
 *
 *  Return log file path.
 */


//----------------------------------------------------------------------------
/*! \fn     int  DApplication::run( int maxproc, bool log )
 *  \param  maxproc Maximum number of processes
 *  \param  log     Enable/disable logging feature
 *  \return success status
 *
 *  Run application. This method only returns when the application ends or
 *  in case of an error condition.
 *
 *  A \a maxproc parameter can be specified to limit the maximum number or
 *  concurrent running instances of this application. 0 means unlimited.
 */

int  DApplication::run( int maxproc, bool log )
{
    ERROR_F( "DApplication::run" );
    m_MaxProcessCount = maxproc;

    if (log && !m_LogPath.empty())
    {
        m_ErrorManager->setLogEnabled( true );
    }

    error << ERRalways << name() << " " << release().asString(true) << endl;

    int  ret = prepare();
    if (ret != 0)
    {
        error << ERRdebug << "prepare() failed with code=" << ret << " exiting." << endl;
        return  ret;
    }
    
    ret = execute();

    finish();

    if (log && !m_LogPath.empty())
    {
        m_ErrorManager->setLogEnabled( false );
    }

    return  ret;
}



//============================================================================
//  P R O T E C T E D   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     bool  DApplication::configure()
 *  \return success state
 *
 *  Configure application internals.
 */

bool  DApplication::configure()
{
    return  true;
}


//----------------------------------------------------------------------------
/*! \fn     int  DApplication::prepare()
 *  \return success status
 *
 *  Prepare for application run.
 */

int  DApplication::prepare()
{
    int  ret = 0;

    if (maxProcessCount() == 1)
    {
        if (!makeUnique())
        {
            return  -1;
        }
    }

    if (m_AppType == D_APP_DAEMON)
    {
        ret = makeDaemon();
    }

    if (!configure())
    {
        return  -1;
    }

    return  ret;
}


//----------------------------------------------------------------------------
/*! \fn     int  DApplication::execute()
 *  \return success status
 *
 *  Execute the real application.
 *
 *  This pure virtual function must be implemented by the user application.
 */


//----------------------------------------------------------------------------
/*! \fn     void  DApplication::finish()
 *
 *  Finish (shutdown) application run.
 */

void  DApplication::finish()
{
}


//----------------------------------------------------------------------------
/*! \fn     bool  DApplication::makeUnique()
 *  \return success state
 *
 *  Try to make this application unique.
 */

bool  DApplication::makeUnique()
{
    ERROR_F( "makeUnique" );

#if defined ( D_OS_UNIX )
    string  pid_file = "/usr/tmp/" + m_AppName + ".pid";
#elif defined ( D_OS_WIN32 )
    string  pid_file = string( getenv("TEMP") ) + "\\" + m_AppName + ".pid";
#else
#   error OS not defined.
#endif 

#if defined ( D_OS_UNIX )
    int  fdes = open( pid_file.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
#elif defined ( D_OS_WIN32 )
    int  fdes = _open( pid_file.c_str(), _O_CREAT| _O_TEMPORARY, _S_IREAD | _S_IWRITE );
#endif // D_OS_UNIX

    if (fdes == -1)
    {
        error << ERRfatal << ERRnx << "Cannot open PID file '" << pid_file << "'" << endl;
        return  false;
    }

#if defined ( D_OS_UNIX )
    struct flock  lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    if (fcntl( fdes, F_SETLK, &lock ) < 0)
    {
        if ((errno == EACCES) || (errno == EAGAIN))
        {
            error << ERRfatal << "Application '" << m_AppName << "' is already running!" << endl;
            return  false;
        }
        else
        {
            error << ERRfatal << ERRnx << "Failed to make unique" << endl;
            return  false;
        }
    }

    if (ftruncate( fdes, (off_t) 0 ) == -1)
    {
        error << ERRfatal << ERRnx << "Failed to make unique" << endl;
        return  false;
    }

#else
    error << ERRwarning << "File locking not implemented" << endl;
#endif // D_OS_UNIX

#if defined ( D_OS_UNIX )
    char  temp[80];
    snprintf( temp, sizeof( temp ), "%ld\n", (long) getpid() );

    if (write( fdes, temp, strlen( temp ) ) != (ssize_t) strlen( temp ))
    {
        error << ERRfatal << ERRnx << "Failed to make unique" << endl;
        return  false;
    }
#elif defined ( D_OS_WIN32 )
#endif // D_OS_UNIX

    return  true;
}


//----------------------------------------------------------------------------
/*! \fn     int  DApplication::makeDaemon()
 *  \return success status
 *
 *  Run application as daemon process.
 */

int  DApplication::makeDaemon()
{
    ERROR_F( "makeDaemon" );

#if defined( D_USE_DAEMONIZE )

    pid_t  pid = fork();
    if (pid == -1)
    {
        return  -1;
    }
    else if (pid != 0)
    {
        exit( 0 );
    }

    setsid();
    signal( SIGHUP, SIG_IGN );

    pid = fork();
    if (pid != 0)
    {
        exit( 0 );
    }

    const char*  path = (directory().empty() ? "/" : directory().c_str());
    chdir( path );

    umask( 0 );

    //  TODO: Close all I/O channels
    //  TODO: Create log file and use handles for cout and cerr
    //  TODO: SYSLOG

#else

    //error << "Daemonize not implemented!" << endl;

#endif  /* D_USE_DAEMONIZE */

    return  0;
}



/********************************** EOF *************************************/
