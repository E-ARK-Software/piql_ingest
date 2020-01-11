/***************************************************************************
**  $Id: dversion.cpp,v 1.1 2005/05/16 09:04:12 swhbt Exp $
**
**  Implementation of the DLFR DVersion version handling class
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
***************************************************************************/

//  PROJECT INCLUDES
//
#include    "dversion.h"
#include    "dstringtools.h"
#include    "derror.h"

//  SYSTEM INCLUDES
//
#include    <sstream>
#include    <string.h>

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )


/****************************************************************************/
/*! \class  DVersion  dversion.h
 *  \ingroup basecommon
 *  \brief  This class provides version handling support.
 *
 *  This class provides version handling support for the DLFR project.
 */


//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     DVersion::DVersion( int major, int minor, int micro, const char* build )
 *  \param  major   Major version number
 *  \param  minor   Minor version number
 *  \param  micro   Micro version number
 *  \param  build   Build date string (__DATE__)
 *
 *  Construct a version handling object.
 */

DVersion::DVersion( int major, int minor, int micro, const char* build )
{
    set( major, minor, micro );
    setBuild( build );
}


//----------------------------------------------------------------------------
/*! \fn     DVersion::DVersion( const std::string& vs )
 *  \param  vs      Version string
 *
 *  Construct a version handling object based on specified version string.
 *  A maximum of two consecutive numbers is extracted from the string and
 *  will be assigned to the major and minor version number resp.
 */

DVersion::DVersion( const std::string& vs )
{
    clear();
    set( vs );
}


//----------------------------------------------------------------------------
/*! \fn     DVersion::DVersion( const DVersion& v )
 *  \param  v       Other version object
 *
 *  Copy constructor.
 */

DVersion::DVersion( const DVersion& v )
{
    _Major = v._Major;
    _Minor = v._Minor;
    _Micro = v._Micro;
    _Build = v._Build;
    _VersionString = v._VersionString;
    _BuildString = v._BuildString;
}


//----------------------------------------------------------------------------
/*! \fn     DVersion&  DVersion::operator = ( int major )
 *  \param  major   Major version number
 *  \return updated object
 *
 *  Assign specified major number to this object and return the updated object.
 */

DVersion&  DVersion::operator = ( int major )
{
    set( major );

    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DVersion&  DVersion::operator = ( const std::string& vs )
 *  \param  vs      Version string
 *  \return updated object
 *
 *  Assign specified version string to this object and return the updated
 *  object.
 */

DVersion&  DVersion::operator = ( const std::string& vs )
{
    set( vs );

    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DVersion&  DVersion::operator = ( const DVersion& v )
 *  \param  v       Other version object
 *  \return updated object
 *
 *  Copy assignment operator.
 */

DVersion&  DVersion::operator = ( const DVersion& v )
{
    if (&v != this)
    {
        _Major = v._Major;
        _Minor = v._Minor;
        _Micro = v._Micro;
        _Build = v._Build;
        _VersionString = v._VersionString;
        _BuildString = v._BuildString;
    }

    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     int  DVersion::majorNumber() const
 *  \return major number part
 *
 *  Return major number part of version.
 */


//----------------------------------------------------------------------------
/*! \fn     int  DVersion::minorNumber() const
 *  \return minor number part
 *
 *  Return minor number part of version.
 */


//----------------------------------------------------------------------------
/*! \fn     int  DVersion::microNumber() const
 *  \return micro number part
 *
 *  Return micro number part of version.
 */


//----------------------------------------------------------------------------
/*! \fn     int  DVersion::buildNumber() const
 *  \return build number part
 *
 *  Return build number part of version.
 */


//----------------------------------------------------------------------------
/*! \fn     std::string  DVersion::asString( bool full ) const
 *  \param  full    Full version string flag
 *  \return version number as string
 *
 *  Return version number as string. The optional argument \a full specifies
 *  if a full string, including build number should be returned.
 */

std::string  DVersion::asString( bool full ) const
{
    std::string  ret = _VersionString;

    if (full && !_BuildString.empty())
        ret += "." + _BuildString;

    return  ret;
}


//----------------------------------------------------------------------------
/*! \fn     void  DVersion::set( int major, int minor, int micro )
 *  \param  major   Major version number
 *  \param  minor   Minor version number
 *  \param  micro   Micro version number
 *
 *  Set version object according to specified values.
 */

void  DVersion::set( int major, int minor, int micro )
{
    _Major = major;
    _Minor = minor;
    _Micro = micro;

    updateString();
}


//----------------------------------------------------------------------------
/*! \fn     void  DVersion::set( const std::string& vs )
 *  \param  vs      Version string
 *
 *  Set version from string.
 */

void  DVersion::set( const std::string& vs )
{
    DStringTools::stringExtractValue( vs, _Major, 1 );
    DStringTools::stringExtractValue( vs, _Minor, 2 );
    DStringTools::stringExtractValue( vs, _Micro, 3 );

    updateString();
}


//----------------------------------------------------------------------------
/*! \fn     void  DVersion::setBuild( const char* date )
 *  \param  date    Compilation date string
 *
 *  Set the build number from specified date string.
 *
 *  This method is to be used in conjunction with the compiler special
 *  name __DATE__.
 */

void  DVersion::setBuild( const char* date )
{
    _Build = 0;

    if ((date != 0) && (strlen( date ) == 11))
    {
        //  We must assume that the format is conform the ANSI C standard
        //
        const char*  months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        int          mdays[] =  {    31,    28,    31,    30,    31,    30,
                                     31,    31,    30,    31,    30,    31 };
        std::string   temp = date;
        unsigned int  mon;

        for (mon=0; mon<12; mon++)
        {
            if (temp.substr( 0, 3 ) == months[mon])
                break;
        }

        if (mon < 12)
        {
            int  year;
            int  day;

            if (DStringTools::stringExtractValue( temp, day, 1 ) && DStringTools::stringExtractValue( temp, year, 2 ))
            {
                _Build = (year % 100) * 1000;

                if ((mon == 2) && ((((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0)))
                    _Build += 1;

                while (mon > 0)
                {
                    _Build += mdays[mon];
                    mon--;
                }

                _Build += day;
            }
        }
    }

    std::ostringstream  strm;
    strm << _Build;

    _BuildString = strm.str();
}


//----------------------------------------------------------------------------
/*! \fn     void  DVersion::clear()
 *
 *  Clear version object. The result is an invalid object.
 */

void  DVersion::clear()
{
    _Major = 0;
    _Minor = 0;
    _Micro = 0;
    _Build = 0;
    _VersionString = "";
    _BuildString = "";
}


//----------------------------------------------------------------------------
/*! \fn     bool  DVersion::isValid() const
 *  \return version object valid state
 *
 *  Return \a true if version object is valid otherwise return \a false.
 */

bool  DVersion::isValid() const
{
    return  !((_Major == 0) && (_Minor == 0));      // easier to read
}



//----------------------------------------------------------------------------
/*! \fn     bool  DVersion::isAtLeast( int major, int minor, int micro ) const
 *  \param  major   Major version value
 *  \param  minor   Minor version value
 *  \param  micro   Micro version value
 *  \return test result
 *
 *  Determine if current version number is at least as specified, in which
 *  case \a true is returned otherwise \a false.
 */

bool  DVersion::isAtLeast( int major, int minor, int micro ) const
{
    if (major < _Major)
        return  true;
    else if (major > _Major)
        return  false;

    if (minor < _Minor)
        return  true;
    else if (minor > _Minor)
        return  false;

    if (micro > _Micro)
        return  false;

    return  true;
}



//============================================================================
//  P R I V A T E   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     void  DVersion::updateString()
 *
 *  Update internal version string.
 */

void  DVersion::updateString()
{
    std::ostringstream  strm;

    strm << _Major << "." << _Minor << "." << _Micro;

    _VersionString = strm.str();
}

//============================================================================
//  G L O B A L   I N T E R F A C E
//============================================================================

DError& D_NAMESPACE::operator<<( DError& err, const D_NAMESPACE::DVersion& version )
{
    err << version.asString( true );  

    return err;
}

bool D_NAMESPACE::operator==( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right )
{
    return
        left.majorNumber() == right.majorNumber() &&
        left.minorNumber() == right.minorNumber() &&
        left.microNumber() == right.microNumber();
}

bool D_NAMESPACE::operator>( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right )
{
    return
        ( left.majorNumber() > right.majorNumber() ) ||
        ( left.majorNumber() == right.majorNumber() && left.minorNumber() > right.minorNumber() ) ||
        ( left.majorNumber() == right.majorNumber() && left.minorNumber() == right.minorNumber() && left.microNumber() > right.microNumber() );
}

bool D_NAMESPACE::operator<( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right )
{
    return !( left == right || left > right );
}

bool D_NAMESPACE::operator>=( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right )
{
    return left > right || left == right;
}

bool D_NAMESPACE::operator<=( const D_NAMESPACE::DVersion& left, const D_NAMESPACE::DVersion& right )
{
    return left < right || left == right;
}

/********************************** EOF *************************************/
