#ifndef     DREGEXP_H
#define     DREGEXP_H

/*****************************************************************************
**
**  Regular expression type definitions
**
**  Creation date:  2012/08/27
**  Created by:     Ole Liabo
**
**
**  Copyright (c) 2012 Piql AS. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/


#include <boost/regex.hpp>

//  NAMESPACE
//
//D_NAMESPACE_BEGIN( D_NAMESPACE )

typedef boost::regex                                            DRegExp;
typedef boost::match_results< std::string::const_iterator >     DRegExpMatch;

class DRegExpUtil
{
public:
    typedef std::vector<std::string> DStringList;

public:
    static bool Valid( const DRegExp& regExp );
    static bool Match( const std::string& str, const std::string& regExp, std::vector<std::string>* matchResults = NULL);
    static bool Match( const std::string& str, const DRegExp& regExp, std::vector<std::string>* matchResults = NULL);
    static bool Replace( std::string& newString, const std::string& str, const std::string& search, const std::string& replace );
    static bool Replace( std::string& newString, const std::string& str, const DRegExp& search, const std::string& replace );
    static bool FindMatchingStrings( std::vector<int>& matches, const DStringList& strings, const std::string& regExp );
    static bool ReplaceMatchingStrings( std::vector<int>& matches, DStringList& replaced, const DStringList& strings, const std::string& search, const std::string& replace );
};


//============================================================================

//D_NAMESPACE_END


#endif  /* DREGEXP_H */
