/*****************************************************************************
**  $Id$
**
**  Implementation of the DError class
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
#include    "derror.h"
#include    "derrorman.h"
#include    "dstringtools.h"
#include    "dtimetools.h"
#include    "dsystemtools.h"

//  SYSTEM INCLUDES
//
#include    <iostream>
#include    <cassert>

#if defined( D_ANSI_CPP_HEADERS )
#   include <cerrno>                // errno
#   include <cstring>               // strerror()
#else
#   include <errno.h>
#   include <string.h>
#endif

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )


/****************************************************************************/
/*! \class  DError  derror.h
 *  \ingroup basecommon
 *  \brief  This class provides a local error stream
 *
 *  This class provides a local error stream. The use of the error stream
 *  class is rather simple. Here is an example:
 *
 *  \code
 *  void  Classname::Method()
 *  {
 *      ERROR_F( "Classname::Method" );     // Create a local error stream
 *      error << ERRdebug << "Starting method Method()" << std::endl;
 *
 *      if (!doSomethingThatShouldWork())
 *          error << "This did not work" << std::endl;
 *
 *      if (!doSomethingThatMayFail())
 *          error << ERRwarning << "Failed, but let's continue" << std::endl;
 *
 *      if (!someSystemFunctionSuccess())
 *          error << ERRnx << "System function failed" << std::endl;
 *
 *      error << ERRinfo << "We completed " << 3 << " tasks" << std::endl;
 *  }
 *  \endcode
 *
 *  The default Type is TYPE_ERROR.
 */

//============================================================================
//  P U B L I C   C O N S T A N T S
//============================================================================

//----------------------------------------------------------------------------
/*! \enum   DError::Type  Enumeration of error types
 */

/*! \var    DError::Type  DError::TYPE_DEBUG       
 *  \brief  Debug entry (no error)
 */

/*! \var    DError::Type  DError::TYPE_INFO       
 *  \brief  Information entry (no error)
 */

/*! \var    DError::Type  DError::TYPE_WARNING    
 *  \brief  Warning (soft error)
 */

/*! \var    DError::Type  DError::TYPE_ERROR      
 *  \brief  Error
 */

/*! \var    DError::Type  DError::TYPE_FATAL      
 *  \brief  Fatal error
 */

/*! \var    DError::Type  DError::TYPE_COUNT
 *  \brief  Number of error types, should not be used as a type.
 */

//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! 
 *  \param  func     Function name
 *  \param  errorMan Log stream, if NULL DErrorMan::Instance is used.
 *  \param  profileAtExit Output profile timings when DError is destructed?
 *
 *  Construct an DError object for specified function.
 */

DError::DError( const std::string& func, DErrorMan* errorMan, bool profileAtExit /*=false*/ )
{
    m_Type = TYPE_ERROR;
    m_FunctionName = func;
    m_ErrorMan = errorMan == NULL ? DErrorMan::Instance() : errorMan;

    if (m_ErrorMan->isIndentingOn())
    {
        m_ErrorMan->pushIndentLevel();
    }

    if (m_ErrorMan->isEntryExitLogging())
    {
        *this << ERRdebug << "Entry" << std::endl;
    }

    if ( profileAtExit )
    {
        m_ProfileTimer = new DErrorTimer( "PROFILE" );
    }
    else
    {
        m_ProfileTimer = NULL;
    }
}


//----------------------------------------------------------------------------
/*! 
 *  \param  cls      Class name
 *  \param  func     Function name
 *  \param  errorMan Log stream, if NULL DErrorMan::Instance is used.
 *  \param  profileAtExit Output profile timings when DError is destructed?
 *
 *  Construct an DError object for specified function.
 */

DError::DError( const std::string& cls, const std::string& func, DErrorMan* errorMan, bool profileAtExit /*=false*/ )
{
    m_Type = TYPE_ERROR;
    m_FunctionName = cls + "::" + func;
    m_ErrorMan = errorMan == NULL ? DErrorMan::Instance() : errorMan;

    m_ErrorMan->pushIndentLevel();

    if (m_ErrorMan->isEntryExitLogging())
    {
        *this << ERRdebug << "Entry" << std::endl;
    }

    if ( profileAtExit )
    {
        m_ProfileTimer = new DErrorTimer( "PROFILE" );
    }
    else
    {
        m_ProfileTimer = NULL;
    }
}


//----------------------------------------------------------------------------
/*!     DError::~DError()
 *
 *  Destructor.
 */

DError::~DError()
{
    if ( m_ProfileTimer )
    {
        *this << ERRinfo << *m_ProfileTimer << endl;
        delete m_ProfileTimer;
    } 

    if (m_ErrorMan->isEntryExitLogging())
    {
        *this << ERRdebug << "Exit" << endl;
    }

    if (m_ErrorMan->isIndentingOn())
    {
        m_ErrorMan->popIndentLevel();
    }
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( error_type_t type )
 *  \param  type    DError type
 *  \return updated error object
 *
 *  Set error type of message. This is only excepted as first manipulator.
 */

DError&  DError::operator << ( error_type_t type )
{
    m_Type = static_cast<int>( type );

    if ( shouldPrint() )
    {
        checkTrailer();
    }

    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( std::ostream& (*manip)( std::ostream& ) )
 *  \param  manip   Stream manipulator
 *  \return updated error object
 *
 *  Process specified manipulator and return updated error object.
 */

DError&  DError::operator << ( std::ostream& (*manip)( std::ostream& ) )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << manip;

        if (m_StreamBuff.str().find( '\n' ) != std::string::npos)
            flush();
    }
    return  *this;
}
    

//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( std::ios_base& (*manip)( std::ios_base& ) )
 *  \param  manip   Stream manipulator
 *  \return updated error object
 *
 *  Process specified manipulator and return updated error object.
 */

DError&  DError::operator << ( std::ios_base& (*manip)( std::ios_base& ) )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << manip;

        if (m_StreamBuff.str().find( '\n' ) != std::string::npos)
            flush();
    }
    return  *this;
}

//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( std::ios& (*manip)( std::ios& ) )
 *  \param  manip
 *  \return updated error object
 *
 *  Process specified manipulator and return updated error object.
 */

DError&  DError::operator << ( std::ios& (*manip)( std::ios& ) )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << manip;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \param  val     field width
 *  \return resulting stream
 *
 *  Add std::setw() to error stream and return resulting stream.
 */
#if !defined (D_OS_X)
DError&  DError::operator << ( SetwType val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}
#endif


//----------------------------------------------------------------------------
/*! \param  val     fill character
 *  \return resulting stream
 *
 *  Add std::setfill() to error stream and return resulting stream.
 */
#if !defined (D_OS_X)
DError&  DError::operator << ( SetfillType val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}
#endif


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( const std::string& str )
 *  \param  str     Message string
 *  \return resulting stream
 *
 *  Add specified message string to error stream and return resulting stream.
 */

DError&  DError::operator << ( const std::string& str )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << str;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( const std::wstring& str )
 *  \param  str     Message string
 *  \return resulting stream
 *
 *  Add specified message wstring to error stream and return resulting stream.
 *  Note that non-ascii characters in the wstring will probably be lost.
 */

DError&  DError::operator << ( const std::wstring& str )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        std::string utf8string;
        DStringTools::toUtf8(str, utf8string);
        m_StreamBuff << utf8string.c_str();
    }
    return  *this;
}

//----------------------------------------------------------------------------
/*! \param  str     Message string
 *  \return resulting stream
 *
 *  Add specified message stringstream to error stream and return resulting stream.
 */

DError&  DError::operator << ( const std::stringstream& str )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << str.str();
    }
    return  *this;
}

//----------------------------------------------------------------------------
/*! \param  str     Message string
 *  \return resulting stream
 *
 *  Add specified message ostringstream to error stream and return resulting stream.
 */

DError&  DError::operator << ( const std::ostringstream& str )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << str.str();
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( const char* str )
 *  \param  str     Message string
 *  \return resulting stream
 *
 *  Add specified message string to error stream and return resulting stream.
 */

DError&  DError::operator << ( const char* str )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << str;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( char* str )
 *  \param  str     Message string
 *  \return resulting stream
 *
 *  Add specified message string to error stream and return resulting stream.
 */

DError&  DError::operator << ( char* str )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << str;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( long long val )
 *  \param  val     Numeric value
 *  \return resulting stream
 *
 *  Add specified numeric value to error stream and return resulting stream.
 */

DError&  DError::operator << ( long long val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( unsigned long long val )
 *  \param  val     Numeric value
 *  \return resulting stream
 *
 *  Add specified numeric value to error stream and return resulting stream.
 */

DError&  DError::operator << ( unsigned long long val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( unsigned long val )
 *  \param  val     Numeric value
 *  \return resulting stream
 *
 *  Add specified numeric value to error stream and return resulting stream.
 */

DError&  DError::operator << ( unsigned long val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( long val )
 *  \param  val     Numeric value
 *  \return resulting stream
 *
 *  Add specified numeric value to error stream and return resulting stream.
 */

DError&  DError::operator << ( long val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( unsigned int val )
 *  \param  val     Numeric value
 *  \return resulting stream
 *
 *  Add specified numeric value to error stream and return resulting stream.
 */

DError&  DError::operator << ( unsigned int val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( int val )
 *  \param  val     Numeric value
 *  \return resulting stream
 *
 *  Add specified numeric value to error stream and return resulting stream.
 */

DError&  DError::operator << ( int val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( float val )
 *  \param  val     Float value
 *  \return resulting stream
 *
 *  Add specified float value to error stream and return resulting stream.
 */

DError&  DError::operator << ( float val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DError&  DError::operator << ( double val )
 *  \param  val     Double value
 *  \return resulting stream
 *
 *  Add specified double value to error stream and return resulting stream.
 */

DError&  DError::operator << ( double val )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << val;
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! 
 *  \param  timer     Error imer 
 *  \return resulting stream
 *
 *  Add timing profiling info to error stream and return resulting stream.
 */

DError&  DError::operator << ( const DErrorTimer& timer )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        m_StreamBuff << timer.debugText() << ":" << timer.elapsed() << " ms";
    }
    return  *this;
}


//----------------------------------------------------------------------------
/*! 
 *  \param  vec     Data vector
 *  \return resulting stream
 *
 *  Add specified vector to error stream, print as hex string.
 */

DError&  DError::operator << ( const std::vector<unsigned char>& vec )
{
    if ( shouldPrint() )
    {
        checkTrailer();

        for ( unsigned int i = 0; i < vec.size(); i++ )
        {
            string hex;
            DStringTools::toString(vec[i], hex, 16);
            m_StreamBuff << hex;
        }
    }
    return  *this;
}

//----------------------------------------------------------------------------
/*! \fn     void  DError::setType( int type )
 *  \param  type    DError type
 *
 *  Set error type. The error type is reset after the end of message.
 */

void  DError::setType( int type )
{
    m_Type = type;
}


//----------------------------------------------------------------------------
/*! \fn     const std::string&  DError::functionName() const
 *  \return Function name
 *
 *  Get function name associated with the DError stream.
 */

//----------------------------------------------------------------------------
/*! \fn     bool  DError::loggingToFile() const
*  \return true if the error will be logged to an actual file
*          false if the error will be logged to a standard stream
*/

bool DError::loggingToFile() const
{
    return m_ErrorMan->loggingToFile();
}


//============================================================================
//  P U B L I C   S T A T I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     std::string  DError::SystemErrorAsString( int code )
 *  \param  code    DError code
 *  \return system error message string
 *
 *  Create and return the error message identified by \a code as string.
 *  In case \a code is specified as \c 0, the current \c errno value is used.
 */

std::string  DError::SystemErrorAsString( int code )
{
    std::string  ret;

    if (code == 0)
        code = errno;

    char*  str = strerror( code );
    if (str != 0)
    {
        std::ostringstream  strm;
        strm << "(errno=" << code << ") " << str << "; ";
        ret = strm.str();
    }

    return  ret;
}


//----------------------------------------------------------------------------
/*! \fn     const char*  DError::TypeAsString( const DError::Type& type )
 *  \param  type   DError type.
 *  \return DError::Type as string, NULL if unknown.
 *
 *  Return the string name for a DError::Type.
 */

const char*  DError::TypeAsString( const DError::Type& type )
{
    switch ( type )
    {
    case TYPE_DEBUG:    return "DEBUG";
    case TYPE_INFO:     return "INFO";
    case TYPE_WARNING:  return "WARNING";
    case TYPE_ERROR:    return "ERROR";
    case TYPE_FATAL:    return "FATAL";
    case TYPE_ALWAYS:   return "ALWAYS";
    case TYPE_COUNT:
        break;
    }
    
    return NULL;
}


//----------------------------------------------------------------------------
/*! \fn     const DError::Type DError::StringToType( const std::string& type )
 *  \param  type   type as string
 *  \return DError::Type, TYPE_INFO if unknown.
 *
 *  Return the a DError::Type for a string.
 */

DError::Type DError::StringToType( const std::string& type )
{
    std::string s = type;
    for (std::string::size_type i=0; i<type.length(); i++)
    {
        s[i] = tolower(type[i]);
    }
    if (s == "debug")
        return TYPE_DEBUG;
    if (s == "info")
        return TYPE_INFO;
    if (s == "warning")
        return TYPE_WARNING;
    if (s == "error")
        return TYPE_ERROR;
    if (s == "fatal")
        return TYPE_FATAL;
    if ( s == "always" )
        return TYPE_ALWAYS;
    assert( false );
    // Defaults to info if the above fails
    return TYPE_INFO;
}


//============================================================================
//  P R I V A T E   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     void  DError::flush()
 *
 *  Flush buffer to output stream and reset internal buffers.
 */

void  DError::flush()
{
    m_ErrorMan->printMessage( m_StreamBuff.str() );

    m_StreamBuff.str( "" );
    m_Type = TYPE_ERROR;
}


//----------------------------------------------------------------------------
/*! \fn     void  DError::insertTrailer()
 *
 *  Insert message trailer.
 */

void  DError::insertTrailer()
{
    if (!m_ErrorMan->moduleName().empty())
        m_StreamBuff << m_ErrorMan->moduleName();

    m_StreamBuff << "[PID:" << static_cast<long>(DSystemTools::processId()) << ",0x"
        << std::hex << std::setw( 5 ) << std::setfill( '0' )
        << DSystemTools::threadId()
        << std::setfill( ' ' ) << std::setw( 0 ) << std::dec << "]";

    switch  (m_Type)
    {
      case  TYPE_DEBUG:
        m_StreamBuff << " -D- ";
        break;

      case  TYPE_INFO:
        m_StreamBuff << " -I- ";
        break;

      case  TYPE_WARNING:
        m_StreamBuff << " -W- ";
        break;

      case  TYPE_ERROR:
        m_StreamBuff << " -E- ";
        break;

      case  TYPE_FATAL:
        m_StreamBuff << " -F- ";
        break;

      case  TYPE_ALWAYS:
        m_StreamBuff << " -A- ";
        break;

      default:
        m_StreamBuff << " -?- ";
        break;
    }

    if (m_ErrorMan->isIndentingOn())
    {
        m_StreamBuff << m_ErrorMan->indentLevel();
    }

    if (!m_FunctionName.empty())
        m_StreamBuff << m_FunctionName << "()-> ";
}


//----------------------------------------------------------------------------
/*! \fn     void  DError::checkTrailer()
 *  \return true if trailes was added, false otherwize.
 *
 *  Check if we have a new message and insert message trailer if needed.
 */

bool  DError::checkTrailer()
{
    if (m_StreamBuff.str().empty())
    {
        if (m_ErrorMan->includeTimeStamp())
            m_StreamBuff << DTimeTools::timeStampLocalAsString() << " ";
        
        insertTrailer();

        return  true;
    }

    return  false;
}


//----------------------------------------------------------------------------
/*! \fn     bool  DError::shouldPrint() const
 *  \return true if the log entry should be printed.
 *
 *  Check if we current filter settings allows this log message to be printed.
 */

bool  DError::shouldPrint() const
{
    return m_ErrorMan->shouldPrint( *this );
}

/****************************************************************************/
/*! \class  DErrorTimer  derror.h
 *  \ingroup basecommon
 *  \brief  This class provides error profiling
 *
 *  This class provides an iunterface that can be used to profile the code
 *  by measuring the time between the object was created and the time when
 *  the object was printed.
 */

//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! 
 *  Constructor.
 */

DErrorTimer::DErrorTimer( const std::string& debugText )
    : m_DebugText( debugText )
{
    m_Start.setFromSystemTime();
}


//----------------------------------------------------------------------------
/*! 
 *  Return time elapsed since construction, in ms.
 */

unsigned int DErrorTimer::elapsed() const
{
    DTimeVal now;
    now.setFromSystemTime();

    return now.msec() - m_Start.msec();
}


//----------------------------------------------------------------------------
/*! 
 *  Return debug info text.
 */

std::string DErrorTimer::debugText() const
{
    return m_DebugText;
}


//----------------------------------------------------------------------------
/*!
 *  Reset start time.
 */
void DErrorTimer::reset()
{
    m_Start.setFromSystemTime();
}

/********************************** EOF *************************************/
