/*****************************************************************************
**
**  PHP utils class
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
#include    "dphputils.h"

//  BASE INCLUDES
//
#include    "dfilesystemtools.h"
#include    "dsystemtools.h"
#include    "dpath.h"

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )


/****************************************************************************/
/*! \class DPhpUtils dphputils.h
 *  \ingroup PiqlIngest
 *  \brief Utility functions for executing PHP scripts.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================


//----------------------------------------------------------------------------
/*! 
 *  Get full path for script
 *
 *  \return Path to script, empty if script does not exist
 */

std::string DPhpUtils::GetScriptPath( const std::string& script )
{
    string path = script;

#if defined (D_OS_X)
    path = DSystemTools::getOsXBundleResource( path );
#endif

    DPath scriptFile( path );

    if ( scriptFile.isValid() && DFileSystemTools::isExistingPath(scriptFile.path()) )
    {
        return path;
    }
    
    return string();
}

//----------------------------------------------------------------------------
/*!
 *  Create script context file, the context file is a generic way of passing
 *  data to the PHP scripts, it contains a XML with key/value parameters.
 *
 *  \return Success state
 */

bool DPhpUtils::CreateScriptContext( DPath& contextPath, const DScriptContext& context, const DPath& tempDir )
{
    ERROR_F("DPhpUtils::CreateScriptContext");

    if ( !DPath::getTempFilename(contextPath, tempDir, "context") )
    {
        error << ERRerror << "Failed to create temp file for context" << endl;
        return false;
    }
    else if ( !context.write(contextPath.path()) )
    {
        error << ERRerror << "Failed to write context" << endl;
        return false;
    }

    return true;
}
