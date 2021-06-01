#ifndef     DSCRIPTCONTEXT_H
#define     DSCRIPTCONTEXT_H

/*****************************************************************************
**
**  Definition of the DSensor class
**
**  Creation date:  2017-07-31
**  Created by:     Matti Wennberg
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the base library
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
D_NAMESPACE_BEGIN( D_NAMESPACE )


//  FORWARD DECLARATIONS
//

//============================================================================
//  CLASS:  DScriptContext
class DScriptContext
{
public:
    DScriptContext();
    ~DScriptContext();

    void addParameterValue(const std::string& key, const std::string& value);
    bool write(const std::string& fileName) const;

private:
    std::vector< std::string >                m_Keys;
    std::vector< std::vector< std::string > > m_Values;
};



//============================================================================

//----------------------------------------------------------------------------
//  INLINE: DScriptContext



//============================================================================

D_NAMESPACE_END


//
//===================================EOF======================================

#endif  /* DSCRIPTCONTEXT_H */
