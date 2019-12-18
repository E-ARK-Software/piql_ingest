#ifndef DPHPUTILS_H
#define DPHPUTILS_H

/*****************************************************************************
**
**  PHP utility functions class
**
**  Creation date:  2017-06-15
**  Created by:     Ole Liabo
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the DLFR software util modules
**
*****************************************************************************/

//  PROJECT INCLUDES
//

//  BASE INCLUDES
//
#include    "derror.h"
#include    "dpath.h"
#include    "dscriptcontext.h"

//  SYSTEM INCLUDES
//
#include    <string>

//  FORWARD DECLARATIONS
//

//============================================================================
// CLASS: DPhpUtils

class DPhpUtils
{
public:

    static std::string GetScriptPath( const std::string& script );
    static bool        CreateScriptContext( DLFR::DPath& contextPath, const DLFR::DScriptContext& context, const DLFR::DPath& tempDir );

private:
    DPhpUtils();
    ~DPhpUtils();
};

//----------------------------------------------------------------------------
// INLINE: DMetadataItem


//====================================EOF=====================================

#endif /* DPHPUTILS_H */
