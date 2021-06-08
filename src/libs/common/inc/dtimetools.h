#ifndef     DTIMETOOLS_H
#define     DTIMETOOLS_H

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
#include    "dcommon.h"

//  SYSTEM INCLUDES
//
#include    <string>

//  NAMESPACE
//
//struct tm;

D_NAMESPACE_BEGIN(D_NAMESPACE)


//============================================================================
//  CLASS:  DTimeTools

class  DTimeTools
{
public:
    static std::string  timeStampUtcAsString();
    static std::string  timeStampAsString(struct tm* time);
    static std::string  timeStampLocalAsString();
    static std::string  timeStampUtcAsFileString();
    static std::string  timeStampLocalAsFileString();
};

D_NAMESPACE_END

//
//===================================EOF======================================

#endif  /* DTIMETOOLS_H */
