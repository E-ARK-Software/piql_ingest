#ifndef     DSTRINGTOOLS_H
#define     DSTRINGTOOLS_H

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

//  PROJECT INCLUDES
//
#include    "dcommon.h"

//  SYSTEM INCLUDES
//
#include    <string>
#include    <vector>

//  NAMESPACE
//

D_NAMESPACE_BEGIN(D_NAMESPACE)


//============================================================================
//  CLASS:  DStringTools

class  DStringTools
{

public:
    static std::string  nameAsFileString(const std::string& name);
    static std::string  mysqlEscapeString(const std::string& str);
    static size_t       hash(const std::string& key);
    static bool         stringEndsWith(const std::string& s1, const std::string& s2, bool ignoreCase = false);
    static int          stringNoCaseCompare(const std::string& s1, const std::string& s2);
    static void         stringToLowerCase(std::string& str);
    static void         stringToUpperCase(std::string& str);
    static void         stringTrimLeft(std::string& str, const std::string& trim = " \t");
    static void         stringTrimRight(std::string& str, const std::string& trim = " \t");
    static void         stringTrimEnds(std::string& str, const std::string& trim = " \t");
    static void         stringReplace(std::string& str, const std::string& replace, const std::string& replaceWith);
    static bool         stringExtractValue(const std::string& str, int& val, int nth = 1);
    static bool         stringExtractValue(const std::string& str, unsigned int& val, int nth = 1);
    static bool         stringExtractValue(const std::string& str, long& val, int nth = 1);
    static bool         stringExtractValue(const std::string& str, double& val, int nth = 1);
    static bool         stringExtractString(const std::string& str, std::string& ret, const std::string& sep, int nth = 1);
    static bool         stringSplit(const std::string& str, std::vector<std::string>& ret, const std::vector<char>& separators);
    static bool         stringSplit(const std::string& str, std::vector<std::string>& ret, const std::string& sep);
    static bool         stringSplit(const std::string& str, std::vector<std::string>& ret, const char sep, const char group = 0);
    static bool         stringSplit(const std::string& str, std::vector<int>& ret, const std::string& sep);
    static bool         toString(int val, std::string& ret, int base = 10);
    static bool         toString(unsigned int val, std::string& ret, int base = 10);
    static bool         toString(long val, std::string& ret, int base = 10);
    static bool         toString(UINT64 val, std::string& ret, int base = 10);
    static bool         toString(double val, std::string& ret, int dec = 2);
    static bool         toInt(const std::string& str, int& ret, int base = 10, bool exactMatch = true);
    static bool         toInt(const std::string& str, unsigned int& ret, int base = 10, bool exactMatch = true);
    static bool         toLong(const std::string& str, long& ret, int base = 10, bool exactMatch = true);
    static bool         toDouble(const std::string& str, double& ret, bool exactMatch = true);
    static bool         toUtf8(const std::wstring& str, std::string& ret);
    static bool         toWstring(const std::string& str, std::wstring& ret);
    static bool         toBool(const std::string& str, bool& ret);
private:
    static const char * locateInt(const std::string& str, int nth, bool sign = true);
    static const char*  locateFloat(const std::string& str, int nth, bool sign = true);
};

D_NAMESPACE_END

//
//===================================EOF======================================

#endif  /* DSTRINGTOOLS_H */
