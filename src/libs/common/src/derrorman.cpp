/*****************************************************************************
**  $Id$
**
**  Implementation of the DErrorMan class
**
**  Creation date:  2004-04-05
**  Created by:     Han Timmerman
**
**  Platform(s):    SGI, Irix 6.5.x, MipsPro 7.3
**                  Intel, Linux 2.4.x, g++ 3.0
**                  Intel, Windows 2K/XP, VC++ 6.0
**
**  Dependencies:   -
**  Restrictions:   -
**  ToDo:           -
**
**  Copyright (c) 2004 ANT Software Development. All rights reserved.
**
**  This file is part of the "cvbase" library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "derrorman.h"
#include    "dthread.h"

//  SYSTEM INCLUDES
//
#include    <fstream>
#include    <iostream>

#if defined( D_ANSI_CPP_HEADERS )
#   include <cstdlib>           // exit()
#else
#   include <stdlib.h>
#   include <string.h>
#endif /* D_ANSI_CPP_HEADERS */

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )

//  GLOBAL INITIALIZATION
//

#if defined D_CC_GNU
# define D_WEAK_SYMBOL __attribute__((weak))
#elif defined D_OS_WIN32
# define D_WEAK_SYMBOL __declspec(selectany)
#else
# define D_WEAK_SYMBOL
#endif


D_WEAK_SYMBOL std::ostream*       DWeakExportLogStream;
D_WEAK_SYMBOL std::ostream*       DWeakExportDefaultStream;
D_WEAK_SYMBOL std::ostream*       DWeakExportErrStream;

DMutex  ioMutex;

/****************************************************************************/
/*! \class  DErrorMan   derrorman.h
 *  \ingroup basecommon
 *  \brief  This is an error handling manager class.
 *
 *  This error manager class handles all error reporting in a central place.
 *
 *  This class is a singleton, get the global instance using Instance().
 *
 *  The error manager holds two streams:
 *   - The default stream m_DefaultStream: Defaults to std::cerr. It can be overridden
 *     with setErrorStream().
 *   - The log stream m_LogStream: Defaults to NULL. The log stream can be set 
 *     with setLogStream(). The log manager will then direct the log to a file 
 *     instead of to the error stream.
 *
 *   The currently active stream is stored in #m_ErrStream, all the log messages are 
 *   written there.
 */

//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     DErrorMan::DErrorMan( const std::string& mod )
 *  \param  mod     Module name
 *
 *  Construct a error manager object.
 */


DErrorMan::DErrorMan( const std::string& mod )
{
    m_ModuleName = mod;
    m_ErrOn = true;
    m_LogOn = false;
    DWeakExportDefaultStream = m_DefaultStream = &std::cerr;
    DWeakExportErrStream = m_ErrStream = m_DefaultStream;
    DWeakExportLogStream = m_LogStream = 0;
    m_LogLevel = DError::TYPE_ERROR;
    m_IndentingOn = false;
    m_EntryExitLogging = false;
}


//----------------------------------------------------------------------------
/*! \fn     DErrorMan::~DErrorMan()
 *
 *  Desctructor.
 */

DErrorMan::~DErrorMan()
{
    clearLogStream();
}


//----------------------------------------------------------------------------
/*! \fn     void    DErrorMan::setOn()
 *
 *  Enable error output stream.
 */


//----------------------------------------------------------------------------
/*! \fn     void    DErrorMan::setOff()
 *
 *  Disable error output stream.
 */

//----------------------------------------------------------------------------
/*! \fn     void    DErrorMan::enableIndenting( bool enable)
 *
 *  Enable/disable indenting of each log statement
 */

//----------------------------------------------------------------------------
/*! \fn     void  DErrorMan::setLogLevel( const DError::Type& logLevel )
 *  \param  logLevel  Only log messages equal to or above this level should be printed
 *
 *  Set log level.
 */


//----------------------------------------------------------------------------
/*! \fn     DError::Type  DErrorMan::logLevel() const
 *  \return Current log level
 *
 *  Get log level.
 */


//----------------------------------------------------------------------------
/*! \fn     void  DErrorMan::addExcludeFilter( const FilterItem& item )
 *  \param  item    Item to exclude from log.
 *
 *  Add log exclude item.
 */

void  DErrorMan::addExcludeFilter( const FilterItem& item )
{
    m_ExcludeFilter.push_back( item );
}


//----------------------------------------------------------------------------
/*! \fn     void  DErrorMan::addIncludeFilter( const FilterItem& item )
 *  \param  item    Item to include from log.
 *
 *  Add log include item.
 */

void  DErrorMan::addIncludeFilter( const FilterItem& item )
{
    m_IncludeFilter.push_back( item );
}


//----------------------------------------------------------------------------
/*! \fn     void  DErrorMan::clearExcludeFilter()
 *
 *  Clear exclude log.
 */

void  DErrorMan::clearExcludeFilter()
{
    m_ExcludeFilter.clear();
}


//----------------------------------------------------------------------------
/*! \fn     void  DErrorMan::clearIncludeFilter()
 *
 *  Clear include log.
 */

void  DErrorMan::clearIncludeFilter()
{
    m_IncludeFilter.clear();
}


//----------------------------------------------------------------------------
/*! \fn     bool  DErrorMan::shouldPrint( const DError& error ) const
 *  \return true if the log entry should be printed.
 *
 *  Check if we current filter settings allows this log message to be printed.
 */

bool  DErrorMan::shouldPrint( const DError& error ) const
{
    if ( !m_ErrOn )
    {
        return false;    
    }
    
    if ( error.type() < logLevel() )
    {
        return false;
    }

    // Check the exclude filter
    size_t excludeSize = m_ExcludeFilter.size();
    for ( unsigned int i = 0; i < excludeSize; i++ )
    {
        if ( strstr( error.functionName().c_str(), m_ExcludeFilter[i].m_String.c_str() ) )
        {
            if ( error.type() <= m_ExcludeFilter[i].m_Level )
            {
                return false;
            }
        }
    }

    // If we don't have any include filters, all items should pass
    size_t includeSize = m_IncludeFilter.size();
    if ( includeSize == 0 )
    {
        return true;
    }
    else
    {
        // If we have include filters, only items that match one of the include filters
        // should pass
        for ( unsigned int i = 0; i < includeSize; i++ )
        {
            if ( strstr( error.functionName().c_str(), m_IncludeFilter[i].m_String.c_str() ) )
            {
                if ( error.type() >= m_IncludeFilter[i].m_Level )
                {
                    return true;
                }
            }
        }
        return false;
    }
    return true;
}


//----------------------------------------------------------------------------
/*! \fn     void    DErrorMan::setModuleName( const std::string& pname )
 *  \param  pname   Program name
 *
 *  Set program name.
 */

void  DErrorMan::setModuleName( const std::string& pname )
{
    m_ModuleName = pname;
}


//----------------------------------------------------------------------------
/*! \fn     bool  DErrorMan::setErrorStream( std::ostream& estrm )
 *  \param  estrm   Error stream
 *
 *  Set error stream.
 */

bool  DErrorMan::setErrorStream( std::ostream& estrm )
{
    DWeakExportErrStream = m_ErrStream = &estrm;
    return true;
}


//----------------------------------------------------------------------------
/*! \fn     bool  DErrorMan::setLogStream( const std::string& fname )
 *  \param  fname   Name of log file
 *  \return success state
 *
 *  Set and create a log file stream.
 */

bool  DErrorMan::setLogStream( const std::string& fname )
{
    if (fname == m_LogFile && m_LogStream != 0)
        return  true;

    clearLogStream();

    m_LogFile = fname;
    DWeakExportLogStream = m_LogStream = new std::ofstream( fname.c_str() );

    return  (m_LogStream != 0 && m_LogStream->good());
}

//----------------------------------------------------------------------------
/*! \fn     bool  DErrorMan::loggingToFile()
*  \return true if logfile is set
*
*  Test if the logger is set to a file (return false if a standard stream is used)
*/
bool DErrorMan::loggingToFile() const
{
    return !m_LogFile.empty();
}

//----------------------------------------------------------------------------
/*! \fn     void  DErrorMan::setLogEnabled( bool state )
 *  \param  state   New log state
 *
 *  Enable or disable logging to file.
 */

void  DErrorMan::setLogEnabled( bool state )
{
    if (m_LogStream == 0)
        return;

    DWeakExportErrStream = m_ErrStream = (state) ? m_LogStream : m_DefaultStream;
}


//----------------------------------------------------------------------------
/*! \fn     void  DErrorMan::clearLogStream()
 *
 *  Close and clear log stream.
 */

void  DErrorMan::clearLogStream()
{
    DWeakExportErrStream = m_ErrStream = m_DefaultStream;

    delete  m_LogStream;
    DWeakExportLogStream = m_LogStream = 0;
}


//----------------------------------------------------------------------------
/*! \fn     bool  DErrorMan::includeTimeStamp()
 *
 *  When logging to file, a timestamp should be included.
 */

bool  DErrorMan::includeTimeStamp()
{
    return m_ErrStream == m_LogStream;
}


//----------------------------------------------------------------------------
/*! \fn     ostream&  DErrorMan::stream()
 *  \return error output stream
 *
 *  Return reference to error output stream.
 */


//----------------------------------------------------------------------------
/*! \fn     std::string&  DErrorMan::moduleName() const
 *  \return module name
 *
 *  Return module name.
 */


//----------------------------------------------------------------------------
/*! 
 *  Increase indenting with one level - should use thread local storage
 */

void  DErrorMan::pushIndentLevel()
{
    DUniqueLock lock(ioMutex);
    if ( isIndentingOn() )
    {
        m_CurrentIndent.push_back( ' ' );
    }
}

//----------------------------------------------------------------------------
/*! 
 *  Decrease indenting with one level.
 */

void  DErrorMan::popIndentLevel()
{
    DUniqueLock lock(ioMutex);
    if ( isIndentingOn() )
    {
        m_CurrentIndent.erase( 0, 1 );
    }
}


//----------------------------------------------------------------------------
/*! 
 *  Print a log statement to the stream. Threadsafe.
 */

void DErrorMan::printMessage( const std::string& message )
{
    DUniqueLock lock(ioMutex);
    stream() << message << std::flush;
}


//==========================================================================
//  P U B L I C   S T A T I C   M E T H O D S
//==========================================================================

//----------------------------------------------------------------------------
/*! \fn     DErrorMan*    DErrorMan::Instance()
 *  \return error manager singleton
 *
 *  Return reference to the default error manager instance.
 */

DErrorMan*   DErrorMan::Instance()
{
    static DErrorMan errorMan( "" );

    return &errorMan;
}


//==========================================================================
//  P R I V A T E   M E T H O D S
//==========================================================================




/********************************** EOF *************************************/
