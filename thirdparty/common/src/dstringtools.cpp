/*****************************************************************************
**
**  Definition of DLFR DStringTools string tools class
**
**  Creation date:  2019/07/10
**  Created by:     Oleksandr Ivanov
**
**
**  Copyright (c) 2019 Piql AS. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/

#define __STDC_FORMAT_MACROS

//  PROJECT INCLUDES
//
#include    "dstringtools.h"
#include    "derror.h"

//  SYSTEM INCLUDES
//
#include    <sys/types.h>
#include    <string.h>                  // strncpy()
#include    <stdlib.h>                  // atoi(), ...
#include    <algorithm>
#include    <stdio.h>


#ifdef D_OS_LINUX
#include    <iconv.h>
#endif

#ifdef D_OS_WIN32
#define     snprintf _snprintf
#endif

//  BOOST INCLUDES
//
#include    <boost/functional/hash.hpp>

//  NAMESPACE
//
D_NAMESPACE_USING(D_NAMESPACE)
D_NAMESPACE_USING(std)

/****************************************************************************/
/*! \class  DStringTools  dstringtools.h
 *  \ingroup basecommon
 *  \brief  This class provides basic string tools.
 *
 */


//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================


//----------------------------------------------------------------------------
/*! \fn     std::string  DStringTools::nameAsFileString( const std::string& name )
 *  \param  name    Name
 *  \return name as filename compatible string
 *
 *  Convert specified name to filename and strip all illegal filename
 *  characters.
 */

 std::string  DStringTools::nameAsFileString(const std::string& name)
{
    std::string  ret;

    if ( name.empty() )
    {
        ret = "noname";
    }
    else
    {
        size_t  spos = 0;

        while ( spos < name.size() )
        {
            if ( (name[spos] >= 'a') && (name[spos] <= 'z') )
            {
                ret += name[spos++];
            }
            else if ( (name[spos] >= 'A') && (name[spos] <= 'Z') )
            {
                ret += name[spos++];
            }
            else if ( (name[spos] >= '0') && (name[spos] <= '9') )
            {
                ret += name[spos++];
            }
            else if ( (name[spos] == '_') || (name[spos] == '-') )
            {
                ret += name[spos++];
            }
            else
            {
                spos++;
            }
        }
    }

    return  ret;
}


//----------------------------------------------------------------------------
/*! 
 *  \param  str    String to modify
 *  \return string with encoding removed.
 *
 *  Secure input data for MySQL database by escaping dangerous characters and return the result.
 */

string  DStringTools::mysqlEscapeString(const std::string& str)
{
    string ret = str;
    stringReplace( ret, "'", "\\'" ); // Escape '
    stringReplace( ret, "\"", "\\\"" ); // Escape "

    return ret;
}


//----------------------------------------------------------------------------
/*! \param  key    Key to hash
 *  \return Hash value of string
 *
 *  Create hash value of string, uses boost::hash.
 */

size_t DStringTools::hash(const std::string& key)
{
    boost::hash<std::string> string_hash;

    return string_hash( key );
}


//----------------------------------------------------------------------------
/*! \param  s1      String to search
 *  \param  ending  String to search for at end of s1
 *  \param  ignoreCase Case-insensitive compare
 *  \return comparison result
 *
 *  Search for match at end of string.
 */

bool DStringTools::stringEndsWith(const std::string& s1, const std::string& ending, bool ignoreCase /*=false*/)
{
    if (s1.length() >= ending.length())
    {
        if (!ignoreCase)
        {
            return s1.compare(s1.length() - ending.length(), ending.length(), ending) == 0;
        }
        else
        {
            string s2 = s1.substr(s1.length() - ending.length(), ending.length());
            return DStringTools::stringNoCaseCompare(s2, ending) == 0;
        }
    }
    else
    {
        return false;
    }
}


//----------------------------------------------------------------------------
/*! \fn     int  DStringTools::stringNoCaseCompare( const std::string& s1, const std::string& s2 )
 *  \param  s1      First string
 *  \param  s2      Second string
 *  \return comparison result
 *
 *  Case insensitive compare of strings.
 */

int  DStringTools::stringNoCaseCompare(const std::string& s1, const std::string& s2)
{
    std::string::const_iterator  iter1 = s1.begin();
    std::string::const_iterator  iter2 = s2.begin();

    while ((iter1 != s1.end()) && (iter2 != s2.end()))
    {
        if (::tolower(*iter1) != ::tolower(*iter2))
        {
            return  ((::tolower(*iter1) < ::tolower(*iter2)) ? -1 : 1);
        }

        ++iter1;
        ++iter2;
    }

    size_t  len1 = s1.length();
    size_t  len2 = s2.length();

    if (len1 == len2)
    {
        return  0;
    }

    if (len1 < len2)
    {
        return  -1;
    }

    return  1;
}


//----------------------------------------------------------------------------
/*! \fn     void  DStringTools::stringToLowerCase( std::string& str )
 *  \param  str     String to be converted
 *
 *  Convert this string to lower case
 */

void  DStringTools::stringToLowerCase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}


//----------------------------------------------------------------------------
/*! \fn     void  DStringTools::stringToUpperCase( std::string& str )
 *  \param  str     String to be converted
 *
 *  Convert this string to upper case
 */

void  DStringTools::stringToUpperCase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}


//----------------------------------------------------------------------------
/*! \param  str     String to be trimmed
 *  \param  trim    Charachters to be removed
 *
 *  Trim left side of specified string. This process removes all whitespace
 *  characters from the left side of the string.
 */

void  DStringTools::stringTrimLeft(std::string& str, const std::string& trim /*= " \t"*/)
{
    size_t  ws_count = 0;
    size_t  idx;

    for (idx = 0; idx<str.length(); idx++, ws_count++)
    {
        bool found = false;
        for (size_t idt = 0; idt<trim.length(); idt++)
        {
            if (str[idx] == trim[idt])
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            break;
        }
    }

    if (ws_count > 0)
    {
        str = str.substr(idx);
    }
}


//----------------------------------------------------------------------------
/*! \param  str     String to be trimmed
 *  \param  trim    Charachters to be removed
 *
 *  Trim right side of specified string. This process removes all whitespace
 *  characters from the right side of the string.
 */

void  DStringTools::stringTrimRight(std::string& str, const std::string& trim /*= " \t"*/)
{
    string::size_type length = str.length();

    if (length == 0)
    {
        return;
    }

    size_t  ws_count = 0;
    size_t  idx = length - 1;

    while (idx > 0)
    {
        bool found = false;
        for (size_t idt = 0; idt<trim.length(); idt++)
        {
            if (str[idx] == trim[idt])
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            break;
        }


        ws_count++;
        idx--;
    }

    if (ws_count > 0)
    {
        str = str.substr(0, str.length() - ws_count);
    }
}


//----------------------------------------------------------------------------
/*! \param  str     String to be trimmed
 *  \param  trim    Charachters to be removed
 *
 *  Trim both ends of specified string. This process removes all non-whitespace
 *  character from both side of the string.
 */

void  DStringTools::stringTrimEnds(std::string& str, const std::string& trim /*= " \t"*/)
{
    stringTrimLeft(str, trim);
    stringTrimRight(str, trim);
}


//----------------------------------------------------------------------------
/*! \fn     void  DStringTools::stringReplace( std::string& str, const std::string& replace, const std::string& replaceWith )
 *  \param  str         String to be modified
 *  \param  replace     String to search for
 *  \param  replaceWith String to insert
 *
 *  Replace all occurances of a substring inside a string.
 */

void  DStringTools::stringReplace(std::string& str, const std::string& replace, const std::string& replaceWith)
{
    size_t nextSearch = 0;
    size_t pos = 0;
    while (pos != string::npos)
    {
        pos = str.find(replace, nextSearch);

        if (pos != string::npos)
        {
            str = str.substr(0, pos) + replaceWith + str.substr(replace.size() + pos);
            nextSearch = pos + replaceWith.size();
        }
    }
}


//----------------------------------------------------------------------------
/*! \fn     bool  DStringTools::stringExtractValue( const std::string& str, int& val, int nth )
 *  \param  str     Source string
 *  \param  val     Reference to return value
 *  \param  nth     Value occurance index
 *  \return success state
 *
 *  Extract and return integer value from specified string.
 */

bool  DStringTools::stringExtractValue(const std::string& str, int& val, int nth)
{
    val = 0;

    if (str.empty())
    {
        return  false;
    }

    if (nth < 1)
    {
        nth = 1;
    }

    const char*  valp = locateInt(str, nth);

    if (valp != 0)
    {
        val = atoi(valp);
    }

    return  (valp != 0);
}


//----------------------------------------------------------------------------
/*! \fn     bool  DStringTools::stringExtractValue( const std::string& str, unsigned int& val, int nth )
 *  \param  str     Source string
 *  \param  val     Reference to return value 
 *  \param  nth     Value occurance index
 *  \return success state
 *
 *  Extract and return integer value from specified string.
 */

bool  DStringTools::stringExtractValue(const std::string& str, unsigned int& val, int nth)
{
    val = 0;

    if (str.empty())
    {
        return  false;
    }

    if (nth < 1)
    {
        nth = 1;
    }

    const char*  valp = locateInt(str, nth);

    if (valp != 0)
    {
        val = atoi(valp);
    }

    return  (valp != 0);
}


//----------------------------------------------------------------------------
/*! \fn     bool  DStringTools::stringExtractValue( const std::string& str, long& val, int nth )
 *  \param  str     Source string
 *  \param  val     Reference to return value
 *  \param  nth     Value occurance index
 *  \return success state
 *
 *  Extract and return long integer value from specified string.
 */

bool  DStringTools::stringExtractValue(const std::string& str, long& val, int nth)
{
    val = 0;

    if (str.empty())
    {
        return  false;
    }

    if (nth < 1)
    {
        nth = 1;
    }

    const char*  valp = locateInt(str, nth);

    if (valp != 0)
    {
        val = atol(valp);
    }

    return  (valp != 0);
}




//----------------------------------------------------------------------------
/*! \fn     bool  DStringTools::stringExtractValue( const std::string& str, double& val, int nth )
 *  \param  str     Source string
 *  \param  val     Reference to return value
 *  \param  nth     Value occurance index
 *  \return success state
 *
 *  Extract and return double value from specified string.
 */

bool  DStringTools::stringExtractValue(const std::string& str, double& val, int nth)
{
    val = 0;

    if (str.empty())
    {
        return  false;
    }

    if (nth < 1)
    {
        nth = 1;
    }

    const char*  valp = locateFloat(str, nth);

    if (valp != 0)
    {
        val = atof(valp);
    }

    return  (valp != 0);
}


//----------------------------------------------------------------------------
/*! \fn     bool  DStringTools::stringExtractString( const std::string& str, std::string& ret, const std::string& sep, int nth )
 *  \param  str     Source string
 *  \param  ret     Return string
 *  \param  sep     Separator string
 *  \param  nth     Separator occurance index
 *  \return success state
 *
 *  Extract and return substring from specified string.
 */

bool  DStringTools::stringExtractString(const std::string& str, std::string& ret, const std::string& sep, int nth)
{
    ret = "";

    size_t  srclen = str.length();
    size_t  seplen = sep.length();

    if (srclen <= seplen)
    {
        return  false;
    }

    if (nth < 1)
    {
        nth = 1;
    }

    size_t  seppos = 0;

    while (nth > 1)
    {
        seppos = str.find(sep, seppos);
        if (seppos == string::npos)
        {
            return  false;
        }

        seppos++;
        if (seppos >= srclen)
        {
            return  false;
        }

        nth--;
    };

    size_t  endpos = str.find(sep, seppos);

    if (endpos == string::npos)
    {
        ret = str.substr(seppos);
    }
    else
    {
        ret = str.substr(seppos, endpos - seppos);
    }

    return  true;
}


//----------------------------------------------------------------------------
/*!
 *  \param  str     Source string
 *  \param  ret     Vector of substrings
 *  \param  sep     Vector of separator chars
 *  \param  group   If not 0, the group char starts and stop a group, inside a group
 *                  the sep is not handled as a separator.
 *  \return success state
 *
 *  Split string into substrings.
 */

bool DStringTools::stringSplit(
    const std::string& str,
    std::vector<std::string>& ret,
    const char sep,
    const char group /*= 0*/)
{
    ret.clear();

    bool inGroup = false;
    size_t strLength = str.length();
    size_t start = 0;
    for (size_t i = 0; i < strLength; i++)
    {
        if (str[i] == group && group != 0 && !inGroup)
        {
            inGroup = true;
        }
        else if (str[i] == group && group != 0 && inGroup)
        {
            inGroup = false;
        }
        else if (str[i] == sep && !inGroup)
        {
            if (start == i)
            {
                start++;
            }
            else
            {
                ret.push_back(str.substr(start, i - start));
                start = i + 1;
            }
        }
    }

    if (start < strLength)
    {
        ret.push_back(str.substr(start, strLength - start));
    }
    return true;
}


//----------------------------------------------------------------------------
/*!
 *  \param  str     Source string
 *  \param  ret     Vector of substrings
 *  \param  sep     Vector of separator chars
 *  \return success state
 *
 *  Split string into substrings.
 */

bool DStringTools::stringSplit(
    const std::string& str,
    std::vector<std::string>& ret,
    const std::vector<char>& sep)
{
    ret.clear();

    size_t strLength = str.length();
    size_t start = 0;
    for (size_t i = 0; i < strLength; i++)
    {
        for (size_t s = 0; s < sep.size(); s++)
        {
            if (str[i] == sep[s])
            {
                if (start == i)
                {
                    start++;
                }
                else
                {
                    ret.push_back(str.substr(start, i - start));
                    start = i + 1;
                }
            }
        }
    }

    if (start < strLength)
    {
        ret.push_back(str.substr(start, strLength - start));
    }
    return true;
}


//----------------------------------------------------------------------------
/*! \fn     bool DStringTools::stringSplit ( const std::string& str, std::vector<std::string>& ret, const std::string& sep )
 *  \param  str     Source string
 *  \param  ret     Vector of substrings
 *  \param  sep     Separator specified as a string
 *  \return success state
 *
 *  Split string into substrings.
 */

bool DStringTools::stringSplit(
    const std::string& str,
    std::vector<std::string>& ret,
    const std::string& sep)
{
    ret.clear();

    string strCpy = str;
    size_t pos = 0;
    while (pos != string::npos)
    {
        pos = strCpy.find(sep);

        if (pos != string::npos)
        {
            // Get content before separator
            string cur = strCpy.substr(0, pos);
            ret.push_back(cur);

            // Remove separator
            strCpy = strCpy.substr(pos + sep.size(), strCpy.size() - pos - sep.size());
        }
    }

    if (strCpy.size() > 0)
    {
        ret.push_back(strCpy);
    }

    return true;
}


//----------------------------------------------------------------------------
/*! \fn     bool DStringTools::stringSplit ( const std::string& str, std::vector<int>& ret, const std::string& sep )
 *  \param  str     Source string
 *  \param  ret     Vector of ints
 *  \param  sep     Separator specified as a string
 *  \return success state
 *
 *  Split string into integers.
 */

bool DStringTools::stringSplit(
    const std::string& str,
    std::vector<int>& ret,
    const std::string& sep)
{
    ret.clear();

    std::vector<string> retString;

    if (!stringSplit(str, retString, sep))
    {
        return false;
    }

    for (unsigned int i = 0; i < retString.size(); i++)
    {
        int v;
        if (!toInt(retString[i], v))
        {
            return false;
        }
        ret.push_back(v);
    }

    return true;
}


//----------------------------------------------------------------------------
/*! \param  val     Value
 *  \param  ret     Return string
 *  \param  base    Conversion base
 *  \return success state
 *
 *  Return specified number as string.
 */

bool  DStringTools::toString(int val, std::string& ret, int base)
{
    char  work[64];

    ret.clear();

    switch (base)
    {
    case  8:
        if (snprintf(work, 63, "%o", static_cast<unsigned int>(val)) < 0)
        {
            return  false;
        }
        break;

    case  10:
        if (snprintf(work, 63, "%d", val) < 0)
        {
            return  false;
        }
        break;

    case  16:
        if (snprintf(work, 63, "%x", static_cast<unsigned int>(val)) < 0)
        {
            return  false;
        }
        break;

    default:
        return  false;
    }

    ret = work;

    return  true;
}


//----------------------------------------------------------------------------
/*! \param  val     Value
 *  \param  ret     Return string
 *  \param  base    Conversion base
 *  \return success state
 *
 *  Return specified number as string.
 */

bool  DStringTools::toString(unsigned int val, std::string& ret, int base)
{
    char  work[64];

    ret.clear();

    switch (base)
    {
    case  8:
        if (snprintf(work, 63, "%o", val) < 0)
        {
            return  false;
        }
        break;

    case  10:
        if (snprintf(work, 63, "%u", val) < 0)
        {
            return  false;
        }
        break;

    case  16:
        if (snprintf(work, 63, "%x", val) < 0)
        {
            return  false;
        }
        break;

    default:
        return  false;
    }

    ret = work;

    return  true;
}


//----------------------------------------------------------------------------
/*! \param  val     Value
 *  \param  ret     Return string
 *  \param  base    Conversion base
 *  \return success state
 *
 *  Return specified number as string.
 */

bool  DStringTools::toString(long val, std::string& ret, int base)
{
    char  work[64];

    ret.clear();

    switch (base)
    {
    case  8:
        if (snprintf(work, 63, "%lo", static_cast<unsigned long>(val)) < 0)
        {
            return  false;
        }
        break;

    case  10:
        if (snprintf(work, 63, "%ld", val) < 0)
        {
            return  false;
        }
        break;

    case  16:
        if (snprintf(work, 63, "%lx", static_cast<unsigned long>(val)) < 0)
        {
            return  false;
        }
        break;

    default:
        return  false;
    }

    ret = work;

    return  true;
}


//----------------------------------------------------------------------------
/*! \param  val     Value
 *  \param  ret     Return string
 *  \param  base    Conversion base
 *  \return success state
 *
 *  Return specified number as string.
 */

bool  DStringTools::toString(UINT64 val, std::string& ret, int base)
{
    char  work[64];

    ret.clear();

#if defined (D_OS_WIN32)
    const char* octalFormat = "%llo";
    const char* decimalFormat = "%lld";
    const char* hexFormat = "%llx";
#else
    const char* octalFormat = "%" PRIo64;
    const char* decimalFormat = "%" PRIu64;
    const char* hexFormat = "%" PRIx64;
#endif

    switch (base)
    {
    case  8:
        if (snprintf(work, 63, octalFormat, val) < 0)
        {
            return  false;
        }
        break;

    case  10:
        if (snprintf(work, 63, decimalFormat, val) < 0)
        {
            return  false;
        }
        break;

    case  16:
        if (snprintf(work, 63, hexFormat, val) < 0)
        {
            return  false;
        }
        break;

    default:
        return  false;
    }

    ret = work;

    return  true;
}


//----------------------------------------------------------------------------
/*! \param  val     Value
 *  \param  ret     Return string
 *  \param  dec     Decimal places
 *  \return success state
 *
 *  Return specified number as string.
 */

bool  DStringTools::toString(double val, std::string& ret, int dec)
{
    char  fmt[8];
    char  work[64];

    ret.clear();

    if (snprintf(fmt, 7, "%%.%df", dec) < 0)
    {
        return  false;
    }

    if (snprintf(work, 63, fmt, val) < 0)
    {
        return  false;
    }

    ret = work;

    return  true;
}


//----------------------------------------------------------------------------
/*!
 *  \param  str         Number string
 *  \param  ret         Return int value
 *  \param  base        Base used for conversion, default 10
 *  \param  exactMatch  true if string is allowed to only contain numeric characters [{+ | –}] [0 [{ x | X }]] [digits]
 *  \return success state
 *
 *  Convert string to int value. Uses strtol().
 */

bool  DStringTools::toInt(const std::string& str, int& ret, int base/*=10*/, bool exactMatch/*=true*/)
{
    if (str.length() == 0)
    {
        return false;
    }

    char*  p_end;

    ret = strtol(str.c_str(), &p_end, base);

    if (exactMatch)
    {
        return p_end == str.c_str() + str.length() && str[0] != ' ' && str[0] != '\t';
    }
    return (p_end != str.c_str());
}


//----------------------------------------------------------------------------
/*!
 *  \param  str     Number string
 *  \param  ret     Return unsigned int value
 *  \param  base    Base used for conversion
 *  \param  exactMatch  true if string is allowed to only contain numeric characters [{+ | –}] [0 [{ x | X }]] [digits]
 *  \return success state
 *
 *  Convert string to unsigned int value.
 */

bool  DStringTools::toInt(const std::string& str, unsigned int& ret, int base/*=10*/, bool exactMatch/*=true*/)
{
    if (str.length() == 0)
    {
        return false;
    }

    char*  p_end;

    ret = strtol(str.c_str(), &p_end, base);

    if (exactMatch)
    {
        return p_end == str.c_str() + str.length() && str[0] != ' ' && str[0] != '\t';
    }
    return (p_end != str.c_str());
}

//----------------------------------------------------------------------------
/*!
 *  \param  str     Number string
 *  \param  ret     Return long value
 *  \param  base    Base used for conversion
 *  \param  exactMatch  true if string is allowed to only contain numeric characters [{+ | –}] [0 [{ x | X }]] [digits]
 *  \return success state
 *
 *  Convert string to long value.
 */

bool  DStringTools::toLong(const std::string& str, long& ret, int base, bool exactMatch/*=true*/)
{
    if (str.length() == 0)
    {
        return false;
    }

    char*  p_end;

    ret = strtol(str.c_str(), &p_end, base);

    if (exactMatch)
    {
        return p_end == str.c_str() + str.length() && str[0] != ' ' && str[0] != '\t';
    }
    return (p_end != str.c_str());
}


//----------------------------------------------------------------------------
/*!
 *  \param  str     Number string
 *  \param  ret     Return double value
 *  \param  exactMatch  true if string is allowed to only contain numeric characters [{+ | –}] [0 [{ x | X }]] [digits]
 *  \return success state
 *
 *  Convert string to double value.
 */

bool  DStringTools::toDouble(const std::string& str, double& ret, bool exactMatch/*=true*/)
{
    if (str.length() == 0)
    {
        return false;
    }

    char*  p_end;

    ret = strtod(str.c_str(), &p_end);

    if (exactMatch)
    {
        return p_end == str.c_str() + str.length() && str[0] != ' ' && str[0] != '\t';
    }

    return (p_end != str.c_str());
}


//----------------------------------------------------------------------------
/*! \fn     bool  DStringTools::toUtf8( const std::wstring& str, string& ret )
 *  \param  str  Input unicode string
 *  \param  ret  Return UTF8 string
 *  \return success state
 *
 *  Convert a unicode string to an UTF8 string
 */

#ifdef D_OS_LINUX
bool  DStringTools::toUtf8(const std::wstring& str, string& ret)
{
    iconv_t cd = iconv_open("UTF-8", "WCHAR_T");

    if (cd == (iconv_t)(-1))
    {
        return false;
    }

    const char* inbuf = (const char*)str.c_str();
    char* outbuf = new char[str.length() * 5];
    char* outbufOrig = outbuf;
    size_t inbytesleft = (str.length() + 1)*sizeof(wchar_t);
    size_t outbytesleft = (str.length() + 1) * 5;

    // Now write out the byte sequence to get into the initial state if this is necessary.
    iconv(cd, NULL, NULL, &outbuf, &outbytesleft);

    size_t retval = iconv(
        cd,
        (char**)&inbuf,
        &inbytesleft,
        &outbuf,
        &outbytesleft);

    ret = outbufOrig;

    delete[] outbufOrig;

    iconv_close(cd);

    return retval != ((size_t)(-1));
}

#elif defined (D_OS_WIN32)

bool  DStringTools::toUtf8(const std::wstring& str, string& ret)
{
    // Size required for the UTF8 string, excluding NULL termination
    int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

    // Convert a unicode string to an UTF8 string
    char *szTo = new char[sizeRequired + 1];
    szTo[sizeRequired] = '\0';
    int r = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, szTo, sizeRequired, NULL, NULL);
    ret = szTo;
    delete[] szTo;
    return r != 0;
}
#endif

//----------------------------------------------------------------------------
/*! \fn     bool  DStringTools::toWstring( const std::wstring& str, string& ret )
 *  \param  str  Input UTF8 string
 *  \param  ret  Return unicode string
 *  \return success state
 *
 *  Convert a UTF8 string to an unicode string
 */

#ifdef D_OS_LINUX
bool  DStringTools::toWstring(const string& str, std::wstring& ret)
{
    ERROR_F("DStringTools::toWstring");

    iconv_t cd = iconv_open("WCHAR_T", "UTF-8");

    if (cd == (iconv_t)(-1))
    {
        error << ERRerror << "iconv_open failed" << endl;
        return false;
    }

    size_t inbytesleft = str.length() + 1;
    size_t outbytesleft = inbytesleft * sizeof(wchar_t);

    const char* inbuf = (const char*)str.c_str();
    wchar_t* outbuf = new wchar_t[outbytesleft];
    char* writeptr = (char*)outbuf;

    // Now write out the byte sequence to get into the initial state if this is necessary.
    iconv(cd, NULL, NULL, &writeptr, &outbytesleft);

    size_t retval = iconv(
        cd,
        (char**)&inbuf,
        &inbytesleft,
        &writeptr,
        &outbytesleft);

    ret = outbuf;

    delete[] outbuf;

    iconv_close(cd);

    return retval != ((size_t)(-1));
}
#elif defined (D_OS_WIN32)

bool  DStringTools::toWstring(const string& str, std::wstring& ret)
{
    // Size required for the UNICODE string, excluding NULL termination
    int sizeRequired = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

    // Convert a UTF8 string to an unicode string
    wchar_t *szTo = new wchar_t[sizeRequired + 1];
    szTo[sizeRequired] = L'\0';
    int r = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, szTo, sizeRequired);
    ret = szTo;
    delete[] szTo;
    return r != 0;
}

#endif


//----------------------------------------------------------------------------
/*! \fn     bool  DStringTools::toBool( const std::string& str, bool& ret )
 *  \param  str     Bool string ( accepts values true or false in any case combination )
 *  \param  ret     Return bool value
 *  \return success state
 *
 *  Convert string to bool value.
 */

bool  DStringTools::toBool(const std::string& str, bool& ret)
{
    string s = str;
    stringToLowerCase(s);
    if (s == "true")
    {
        ret = true;
        return true;
    }
    if (s == "false")
    {
        ret = false;
        return true;
    }
    return false;
}

//============================================================================
//  P R I V A T E   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     const char*  DStringTools::locateInt( const std::string& str, int nth, bool sign )
 *  \param  str     Source string
 *  \param  nth     Value occurrence index
 *  \param  sign    Signed value flag
 *  \return pointer to start of value in string
 *
 *  Locate the \a nth occurrance of an integer value in the specified source
 *  string and return a pointer to the start of that buffer position.
 */

const char*  DStringTools::locateInt(const std::string& str, int nth, bool sign)
{
    const char*  pdata = str.c_str();
    const char*  start = 0;
    int          count = 0;
    size_t       index = 0;

    while (index < str.length())
    {
        if (isdigit(pdata[index]))
        {
            if (start == 0)
            {
                start = str.data() + index;
            }
        }
        else
        {
            if (start != 0)
            {
                if (++count < nth)
                {
                    start = 0;
                }
                else
                {
                    break;
                }
            }
        }

        index++;
    }

    if (start == 0)
    {
        return  0;
    }

    if ((index >= str.length()) && (count != (nth - 1)))            // HBT001
    {
        return  0;                                                  // HBT001
    }

    if (sign && (start > str.data()) && (*(start - 1) == '-'))
    {
        start--;
    }

    return  start;
}


//----------------------------------------------------------------------------
/*! \fn     const char*  DStringTools::locateFloat( const std::string& str, int nth, bool sign )
 *  \param  str     Source string
 *  \param  nth     Value occurrence index
 *  \param  sign    Signed value flag
 *  \return pointer to start of value in string
 *
 *  Locate the \a nth occurrance of an double value in the specified source
 *  string and return a pointer to the start of that buffer position.
 */

const char*  DStringTools::locateFloat(const std::string& str, int nth, bool sign)
{
    const char*  pdata = str.c_str();
    const char*  start = 0;
    int          count = 0;
    size_t       index = 0;

    while (index < str.length())
    {
        if (isdigit(pdata[index]) || (pdata[index] == '.') || ((pdata[index] == 'e') && start != 0) || ((pdata[index] == '-') && start != 0))
        {
            if (start == 0)
            {
                start = str.data() + index;
            }
        }
        else
        {
            if (start != 0)
            {
                if (++count < nth)
                {
                    start = 0;
                }
                else
                {
                    break;
                }
            }
        }

        index++;
    }

    if (start == 0)
    {
        return  0;
    }

    if (sign && (index > 0) && (*(start - 1) == '-'))
    {
        start--;
    }

    return  start;
}

/********************************** EOF *************************************/
