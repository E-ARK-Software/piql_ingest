#ifndef     DTIMEVAL_H
#define     DTIMEVAL_H

/*****************************************************************************
**  $Id: dtimeval.h,v 1.1 2004/04/15 19:27:31 swhbt Exp $
**
**  Definition of DLFR DTimeVal timeval wrapper class
**
**  Creation date:  2004/04/14
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
#if defined ( D_OS_UNIX )
#include    <sys/time.h>
#endif // D_OS_UNIX

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )

//  FORWARD DECLARATIONS
//
class DError;

//============================================================================
//  CLASS:  DTimeVal

class  DTimeVal : public timeval
{

public:

    DTimeVal();
    DTimeVal( long sec, long usec=0 );
    DTimeVal( const timeval& tv );

    DTimeVal&       operator  = ( long sec );
    DTimeVal&       operator  = ( const timeval& tv );
    DTimeVal&       operator += ( const timeval& tv );
    DTimeVal&       operator -= ( const timeval& tv );
    DTimeVal        operator +  ( const timeval& tv );
    DTimeVal        operator -  ( const timeval& tv );

    DTimeVal&       set( long sec=0, long usec=0 );
    DTimeVal&       setMs( long msec );
    DTimeVal&       setFromSystemTime();
    void            clear();

    long            sec() const;
    long            msec() const;
    long            usec() const;

    bool            isZero() const;

    int             compare( const timeval& tv ) const;

    bool            operator == ( const timeval& tv ) const;
    bool            operator != ( const timeval& tv ) const;
    bool            operator <  ( const timeval& tv ) const;
    bool            operator <= ( const timeval& tv ) const;
    bool            operator >  ( const timeval& tv ) const;
    bool            operator >= ( const timeval& tv ) const;


//public:     // STATIC

    static DTimeVal ZERO;

};



//----------------------------------------------------------------------------
//  INLINE: DTimeVal

inline  DTimeVal::DTimeVal()
{
    tv_sec = 0;
    tv_usec = 0;
}


inline  DTimeVal::DTimeVal( long sec, long usec )
{
    set( sec, usec );
}


inline  DTimeVal::DTimeVal( const timeval& tv )
{
    set( tv.tv_sec, tv.tv_usec );
}


inline DTimeVal&  DTimeVal::operator = ( long sec )
{
    set( sec );

    return  *this;
}


inline DTimeVal&  DTimeVal::operator = ( const timeval& tv )
{
    set( tv.tv_sec, tv.tv_usec );

    return  *this;
}


inline void  DTimeVal::clear()
{
    tv_sec = 0;
    tv_usec = 0;
}


inline long  DTimeVal::sec() const
{
    return  tv_sec;
}


inline long  DTimeVal::msec() const
{
    return  ((tv_sec * 1000) + (tv_usec / 1000));
}


inline long  DTimeVal::usec() const
{
    return  tv_usec;
}


inline bool  DTimeVal::isZero() const
{
    return  ((tv_sec == 0) && (tv_usec == 0));
}


inline bool  DTimeVal::operator == ( const timeval& tv ) const
{
    return  (compare( tv ) == 0);
}


inline bool  DTimeVal::operator != ( const timeval& tv ) const
{
    return  (compare( tv ) != 0);
}


inline bool  DTimeVal::operator <  ( const timeval& tv ) const
{
    return  (compare( tv ) < 0);
}


inline bool  DTimeVal::operator <= ( const timeval& tv ) const
{
    return  (compare( tv ) <= 0);
}


inline bool  DTimeVal::operator >  ( const timeval& tv ) const
{
    return  (compare( tv ) > 0);
}

inline bool  DTimeVal::operator >= ( const timeval& tv ) const
{
    return  (compare( tv ) >= 0);
}


//============================================================================

DError& operator<<( DError& err, const D_NAMESPACE::DTimeVal& timeval );


//============================================================================

D_NAMESPACE_END


//
//===================================EOF======================================

#endif  /* DTIMEVAL_H */
