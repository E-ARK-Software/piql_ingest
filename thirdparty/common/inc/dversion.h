#ifndef     DVERSION_H
#define     DVERSION_H

/*****************************************************************************
**  $Id: dversion.h,v 1.1 2004/05/16 08:53:02 swhbt Exp $
**
**  Definition of DLFR DVersion version handling class
**
**  Creation date:  2004/04/06
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
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "dcommon.h"

//  SYSTEM INCLUDES
//
#include    <string>

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )

class DError;

//============================================================================
//  CLASS:  DVersion

class  DVersion
{

public:

    DVersion( int major=0, int minor=0, int micro=0, const char* build=0 );
    DVersion( const std::string& vs );
    DVersion( const DVersion& v );

    DVersion&       operator = ( int major );
    DVersion&       operator = ( const std::string& vs );
    DVersion&       operator = ( const DVersion& v );

    int             majorNumber() const;
    int             minorNumber() const;
    int             microNumber() const;
    int             buildNumber() const;
    std::string     asString( bool full=false ) const;

    void            set( int major, int minor=0, int micro=0 );
    void            set( const std::string& vs );
    void            setBuild( const char* date );
    void            clear();

    bool            isValid() const;
    bool            isAtLeast( int major, int minor=0, int micro=0 ) const;

private:

    void            updateString();


private:

    int             _Major;
    int             _Minor;
    int             _Micro;
    int             _Build;
    std::string     _VersionString;
    std::string     _BuildString;

};


//----------------------------------------------------------------------------
//  INLINE: DVersion

inline int  DVersion::majorNumber() const
{
    return  _Major;
}


inline int  DVersion::minorNumber() const
{
    return  _Minor;
}


inline int  DVersion::microNumber() const
{
    return  _Micro;
}


inline int  DVersion::buildNumber() const
{
    return  _Build;
}


//============================================================================

DError& operator<<( DError& err, const D_NAMESPACE::DVersion& version );
bool operator==( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right );
bool operator>( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right );
bool operator<( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right );
bool operator>=( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right );
bool operator<=( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right );

//============================================================================

D_NAMESPACE_END


//
//===================================EOF======================================

#endif  /* DVERSION_H */
