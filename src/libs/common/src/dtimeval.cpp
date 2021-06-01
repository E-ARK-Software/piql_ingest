/***************************************************************************
**  $Id: dtimeval.cpp,v 1.1 2005/05/09 15:10:21 swhbt Exp $
**
**  Implementation of the DLFR DTimeVal timeval wrapper class
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
***************************************************************************/

//  PROJECT INCLUDES
//
#include    "dtimeval.h"
#include    "derror.h"

#if defined ( D_OS_WIN32 )
#   include <time.h>
#   include <sys/timeb.h>
#endif // D_OS_WIN32

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )

//  STATIC INITIALIZATION
//
DTimeVal  DTimeVal::ZERO;


/****************************************************************************/
/*! \class  DTimeVal  dtimeval.h
 *  \ingroup basecommon
 *  \brief  This class is a wrapper class for the timeval structure.
 *
 *  This class is a wrapper class and provides easy timeval handling.
 */


//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     DTimeVal::DTimeVal()
 *
 *  Default constructor. Create a zero DTimeVal object.
 */



//----------------------------------------------------------------------------
/*! \fn     DTimeVal&  DTimeVal::operator += ( const timeval& tv )
 *  \param  tv      other timeval structure
 *  \return updated DTimeVal object
 *
 *  Add specified timeval to this object and return result.
 */

DTimeVal&  DTimeVal::operator += ( const timeval& tv )
{
    tv_usec += tv.tv_usec;
    tv_sec += tv.tv_sec + tv_usec / 1000000;
    tv_usec %= 1000000;

    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DTimeVal&  DTimeVal::operator -= ( const timeval& tv )
 *  \param  tv      other timeval structure
 *  \return updated DTimeVal object
 *
 *  Subtract specified timeval from this object and return result.
 */

DTimeVal&  DTimeVal::operator -= ( const timeval& tv )
{
    tv_sec -= tv.tv_sec;

    if (tv_usec < tv.tv_usec)
    {
        tv_usec += 1000000 - tv.tv_usec;
        tv_sec--;
    }
    else
    {
        tv_usec -= tv.tv_usec;
    }

    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DTimeVal  DTimeVal::operator + ( const timeval& tv )
 *  \param  tv      Other timeval structure
 *  \return result
 *
 *  Add.
 */

DTimeVal  DTimeVal::operator + ( const timeval& tv )
{
    DTimeVal  ret( *this );
    ret += tv;

    return  ret;
}


//----------------------------------------------------------------------------
/*! \fn     DTimeVal  DTimeVal::operator - ( const timeval& tv )
 *  \param  tv      Other timeval structure
 *  \return result
 *
 *  Subtract.
 */

DTimeVal  DTimeVal::operator - ( const timeval& tv )
{
    DTimeVal  ret( *this );
    ret -= tv;

    return  ret;
}


//----------------------------------------------------------------------------
/*! \fn     DTimeVal&  DTimeVal::set( long sec, long usec )
 *  \param  sec     Seconds
 *  \param  usec    Microseconds
 *  \return updated object
 *
 *  Set this object according the specified values and return updated object.
 */

DTimeVal&  DTimeVal::set( long sec, long usec )
{
    tv_sec = sec + usec / 1000000;
    tv_usec = usec % 1000000;

    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DTimeVal&  DTimeVal::setMs( long msec )
 *  \param  msec    Milliseconds
 *  \return updated object
 *
 *  Set this object according the specified value and return updated object.
 */

DTimeVal&  DTimeVal::setMs( long msec )
{
    return  set( 0, msec * 1000 );
}


//----------------------------------------------------------------------------
/*! \fn     DTimeVal&  DTimeVal::setFromSystemTime()
 *  \return updates object
 *
 *  Set this object from curent system time and return updated object.
 */

DTimeVal&  DTimeVal::setFromSystemTime()
{
#if defined ( D_OS_UNIX )

    gettimeofday( this, NULL );

#elif defined ( D_OS_WIN32 )

    struct _timeb timebuffer;
    _ftime64_s( &timebuffer ); 

    tv_sec = (long)timebuffer.time;
    tv_usec = timebuffer.millitm * 1000;

#endif // D_OS_UNIX

    return  *this;
}




//----------------------------------------------------------------------------
/*! \fn     int  DTimeVal::compare( const timeval& tv ) const
 *  \param  tv      Other timeval structure
 *  \return comparison result
 *
 *  Compare specified timeval structure with this object and return -1 if
 */

int  DTimeVal::compare( const timeval& tv ) const
{
    if (tv_sec < tv.tv_sec)
        return  -1;
    else if (tv_sec > tv.tv_sec)
        return   1;
    else if (tv_usec < tv.tv_usec)
        return  -1;
    else if (tv_usec > tv.tv_usec)
        return   1;
    else
        return   0;
}


//============================================================================
//  G L O B A L   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! Write DTimeVal to error stream 
 * 
 *  \param err          Error stream
 *  \param timeval     Timeval to write
 *
 *  Write the DTimeVal to a DError stream
 */

DError& D_NAMESPACE::operator<<( DError& err, const D_NAMESPACE::DTimeVal& timeval )
{
    err << timeval.msec();
    err << " ms";
    return err;
}



/********************************** EOF *************************************/
