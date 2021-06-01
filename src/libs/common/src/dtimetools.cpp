/*****************************************************************************
**
**  Definition of DLFR DTimeTools time tools class
**
**  Creation date:  2019/08/15
**  Created by:     Oleksandr Ivanov
**
**
**  Copyright (c) 2019 Piql AS. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "dtimetools.h"

#if defined ( D_OS_UNIX )
#include    <sys/time.h>                // timeval
#elif defined ( D_OS_WIN32 )
#include    <time.h>
#endif // D_OS_UNIX

//  NAMESPACE
//
D_NAMESPACE_USING(D_NAMESPACE)
D_NAMESPACE_USING(std)

/****************************************************************************/
/*! \class  DTimeTools  dtimetools.h
 *  \ingroup basecommon
 *  \brief  This class provides basic time tools.
 *
 */


//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     std::string  DTimeTools::timeStampUtcAsString()
 *  \return timestamp (utc) as string
 *
 *  Get current utc date and time and return as timestamp string.
 */

 std::string  DTimeTools::timeStampUtcAsString()
{
    std::string  ret;

    time_t  sys_time = time( 0 );
    if ( sys_time == static_cast<time_t>( -1 ) )
    {
        return  ret;
    }

    return timeStampAsString( gmtime( &sys_time ) );
}


//----------------------------------------------------------------------------
/*! 
 *  \param  time    time
 *  \return timestamp as string
 *
 *  Get date and time and return as timestamp string.
 *  Format: YYYY-MM-DD HH:MM:SS
 */

std::string  DTimeTools::timeStampAsString(struct tm* time)
{
    std::string  ret;

    char       str_time[32];
    if ( strftime( str_time, 31, "%Y-%m-%d %H:%M:%S", time ) > 0 )
    {
        ret = str_time;
    }

    return  ret;
}

//----------------------------------------------------------------------------
/*! \fn     std::string  DTimeTools::timeStampLocalAsString()
 *  \return timestamp (local) as string
 *
 *  Get current local date and time and return as timestamp string.
 *  Format: YYYY-MM-DD HH:MM:SS
 */

std::string  DTimeTools::timeStampLocalAsString()
{
    std::string  ret;

    time_t  sys_time = time( 0 );
    if ( sys_time == static_cast<time_t>( -1 ) )
    {
        return  ret;
    }

    return timeStampAsString( localtime( &sys_time ) );
}


//----------------------------------------------------------------------------
/*! \fn     std::string  DTimeTools::timeStampUtcAsFileString()
 *  \return timestamp (utc) as file string
 *
 *  Get current utc date and time and return as timestamp string in a format that
 *  can be used in filenames.
 */

std::string  DTimeTools::timeStampUtcAsFileString()
{
    std::string  ret;

    time_t  sys_time = time( 0 );
    if ( sys_time == static_cast<time_t>( -1 ) )
    {
        return  ret;
    }

    struct tm* utc_time = gmtime( &sys_time );
    char       str_time[32];
    if ( strftime( str_time, 31, "%Y%m%d%H%M%S", utc_time ) > 0 )
    {
        ret = str_time;
    }

    return  ret;
}


//----------------------------------------------------------------------------
/*! \fn     std::string  DTimeTools::timeStampLocalAsFileString()
 *  \return timestamp (local) as file string
 *
 *  Get current local date and time and return as timestamp string in a format that
 *  can be used in filenames.
 */

std::string  DTimeTools::timeStampLocalAsFileString()
{
    std::string  ret;

    time_t  sys_time = time( 0 );
    if ( sys_time == static_cast<time_t>( -1 ) )
    {
        return  ret;
    }

    struct tm* local_time = localtime( &sys_time );
    char       str_time[32];
    if ( strftime( str_time, 31, "%Y%m%d%H%M%S", local_time ) > 0 )
    {
        ret = str_time;
    }

    return  ret;
}

/********************************** EOF *************************************/
