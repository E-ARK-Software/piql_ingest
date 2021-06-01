#ifndef     DERROR_H
#define     DERROR_H

/*****************************************************************************
**  $Id: error.h,v 1.1 2006-03-16 11:11:00 swhbt Exp $
**
**  Definition of the Piql DError class
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
#include    "dcommon.h"
#include    "dtimeval.h"

//  SYSTEM INCLUDES
//
#include    <iosfwd>
#include    <string>
#include    <sstream>
#include    <vector>
#include    <iomanip>

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )
D_NAMESPACE_USE( std::ostream )

//  FORWARD DECLARATIONS
//
class DErrorTimer;
class DErrorMan;


#if defined (D_OS_X )
#elif defined( D_OS_UNIX )
typedef std::_Setw                      SetwType;
typedef std::_Setfill<char>             SetfillType;
#else
typedef std::_Smanip<std::streamsize>   SetwType;
typedef std::_Fillobj<char>             SetfillType;
#endif

//============================================================================
//  CLASS:  DError

class  DError
{

public:

    enum  Type  { TYPE_DEBUG, TYPE_INFO, TYPE_WARNING, TYPE_ERROR, TYPE_FATAL, TYPE_ALWAYS, TYPE_COUNT };
    typedef enum Type   error_type_t;


public:

    DError( const std::string& func, DErrorMan* errorMan=NULL, bool profileAtExit=false );
    DError( const std::string& cls, const std::string& func, DErrorMan* errorMan=NULL, bool profileAtExit=false );
   ~DError();

    DError&             operator << ( error_type_t type );
    DError&             operator << ( std::ostream& (*manip)( std::ostream& ) );
    DError&             operator << ( std::ios_base& (*manip)( std::ios_base& ) );
    DError&             operator << ( std::ios& (*manip)( std::ios& ) );
#if !defined (D_OS_X )
    DError&             operator << ( SetwType val );
    DError&             operator << ( SetfillType val );
#endif
    DError&             operator << ( const std::string& );
    DError&             operator << ( const std::stringstream& );
    DError&             operator << ( const std::ostringstream& );
    DError&             operator << ( const std::wstring& );
    DError&             operator << ( const char* str );
    DError&             operator << ( char* str );
    DError&             operator << ( long long val );
    DError&             operator << ( unsigned long long val );
    DError&             operator << ( unsigned long val );
    DError&             operator << ( long val );
    DError&             operator << ( unsigned int val );
    DError&             operator << ( int val );
    DError&             operator << ( float val );
    DError&             operator << ( double val );
    DError&             operator << ( const DErrorTimer& timer );
    DError&             operator << ( const std::vector<unsigned char>& vec );

    int                 type() const;
    void                setType( int type );
    const std::string&  functionName() const;
    bool                loggingToFile() const;

public:     // STATIC

    static std::string  SystemErrorAsString( int code=0 );
    static const char*  TypeAsString( const DError::Type& type );
    static DError::Type StringToType( const std::string& type );

private:

    void                flush();
    bool                checkTrailer();
    void                insertTrailer();
    bool                shouldPrint() const;

private:

    int                 m_Type;
    std::string         m_FunctionName;
    std::ostringstream  m_StreamBuff;
    DErrorMan*          m_ErrorMan;
    DErrorTimer*        m_ProfileTimer;

private:    // DISABLE

    DError( const DError& );
    DError&  operator = ( const DError& );

};


//----------------------------------------------------------------------------
//  INLINE: DError

inline int  DError::type() const
{
    return  m_Type;
}

inline const std::string&  DError::functionName() const
{
    return  m_FunctionName;
}

//============================================================================
//  CLASS:  DErrorTimer

class  DErrorTimer
{
public:
    DErrorTimer( const std::string& debugText );

    unsigned int    elapsed() const;
    std::string     debugText() const;
    void            reset();

private:
    DTimeVal        m_Start;
    std::string     m_DebugText;
};



//============================================================================
//  MACROS

// DEFINE LOCAL ERROR OBJECT IN GLOBAL FUNCTIONS
//
#define ERROR_F( F )        D_NAMESPACE::DError error( F )
#define ERROR_MF( F )       D_NAMESPACE::DError error( className(), F )
#define ERROR_P( F )        D_NAMESPACE::DError error( F, NULL, true )

#define ERRnx               D_NAMESPACE::DError::SystemErrorAsString()
#define ERRno               D_NAMESPACE::DError::SystemErrorAsString(0)
#define ERRdebug            D_NAMESPACE::DError::TYPE_DEBUG
#define ERRinfo             D_NAMESPACE::DError::TYPE_INFO
#define ERRwarning          D_NAMESPACE::DError::TYPE_WARNING
#define ERRerror            D_NAMESPACE::DError::TYPE_ERROR
#define ERRfatal            D_NAMESPACE::DError::TYPE_FATAL
#define ERRalways           D_NAMESPACE::DError::TYPE_ALWAYS

// Uncomment flags below to enable extended profiling
//#define UNBOXING_PROFILE

//============================================================================

D_NAMESPACE_END


//
//===================================EOF======================================

#endif  /* DERROR_H */
