/*****************************************************************************
**
**  Implementation of RegExp helper classes
**
**  Creation date:  2014/10/23
**  Created by:     Ole Liabo
**
**
**  Copyright (c) 2014 Piql AS. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "dregexp.h"
#include    "derror.h"

//  REGEXP INCLUDES
//

//  SYSTEM INCLUDES
//
#include    <string>

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( boost )


/****************************************************************************/
/*! \class  DRegExpUtil  dregexp.h
 *  \ingroup basecommon
 *  \brief  Utility functions for regexp matching.
 *
 *  Utilitiy functions that simplify regexp mathcing.
 */

bool DRegExpUtil::Valid( const DRegExp& regExp )
{
    return regExp.status() == 0;
}

bool DRegExpUtil::Match( const std::string& str, const std::string& regExp, std::vector<std::string>* matchResults /*= NULL*/)
{
    DRegExp exp( regExp.c_str() );
    if (exp.status() != 0)
    {
        return false;
    }

    return Match( str, exp, matchResults );
}


bool DRegExpUtil::Match( const std::string& str, const DRegExp& regExp, std::vector<std::string>* matchResults /*= NULL*/)
{
    cmatch res;
    if ( regex_match( str.c_str(), res, regExp ) )
    {
        if ( matchResults )
        {
            for ( unsigned int i = 1; i < res.size(); i++ )
            {
                matchResults->push_back( res[i] );
            }
        }
        return true;
    }
    return false;
}


bool DRegExpUtil::Replace( std::string& newString, const std::string& str, const std::string& search, const std::string& replace )
{
    DRegExp exp( search.c_str() );
    if (exp.status() != 0)
    {
        return false;
    }

    return Replace( newString, str, exp, replace );
}


bool DRegExpUtil::Replace( std::string& newString, const std::string& str, const DRegExp& search, const std::string& replace )
{
    newString = boost::regex_replace( str, search, replace );    
    return true;
}



//----------------------------------------------------------------------------
/*! 
 *  Create vector of indices of matching strings.
 *  \return false if regExp is illegal
 */

bool DRegExpUtil::FindMatchingStrings( std::vector<int>& matches, const DStringList& strings, const std::string& regExp )
{
    DRegExp exp( regExp.c_str() );
    if (exp.status() != 0)
    {
        return false;
    }

    matches.clear();
    std::vector<string>::const_iterator it = strings.begin();
    for ( ; it != strings.end(); it++ )
    {
        if ( regex_match( *it, exp ) )
        {
            matches.push_back( static_cast<int>(it - strings.begin()) );
        }
    }
    
    return true;
}


//----------------------------------------------------------------------------
/*! 
 *  Create vector of indices of matching strings and vector of replaced strings
 *  \return false if regExp is illegal
 */

bool DRegExpUtil::ReplaceMatchingStrings( std::vector<int>& matches, DStringList& replaced, const DStringList& strings, const std::string& search, const::string& replace )
{
    DRegExp exp( search.c_str() );
    if (exp.status() != 0)
    {
        return false;
    }

    matches.clear();
    replaced.clear();
    
    std::vector<string>::const_iterator it = strings.begin();
    for ( ; it != strings.end(); it++ )
    {
        std::string newString;
        if ( !DRegExpUtil::Replace( newString, *it, search, replace ) )
        {
            return false;
        }
        
        if ( newString != *it )
        {
            matches.push_back( static_cast<int>(it - strings.begin()) );
            replaced.push_back( newString );
        }
    }

    return true;
}


