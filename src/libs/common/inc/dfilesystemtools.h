#ifndef     DFILESYSTEMTOOLS_H
#define     DFILESYSTEMTOOLS_H

/*****************************************************************************
 **
 **  Definition of DLFR DFileSystemTools tools class
 **
 **  Creation date:  2019/08/13
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
struct tm;

D_NAMESPACE_BEGIN(D_NAMESPACE)


//============================================================================
//  CLASS:  DFileSystemTools

class  DFileSystemTools
{
public:
    static bool isExistingPath(const std::string& path);
    static bool isReadablePath(const std::string& path);
    static bool isExecutableFile(const std::string& fn);
    static bool compareFiles(const std::string& fileA, const std::string& fileB);
    static bool copyFolder(const std::string& source, const std::string& destination);
    static bool readFileIntoString(std::string& text, const std::string& source);
    static bool readFileIntoVector(std::vector<std::string>& lines, const std::string& source);
    static bool readStreamIntoVector(std::vector<std::string>& lines, std::istream& source);
    static bool freeDiskSpace(unsigned long long& freeBytes, unsigned long long& totalBytes, const std::string& folder);
};


D_NAMESPACE_END

//
//===================================EOF======================================

#endif  /* DFILESYSTEMTOOLS_H */
