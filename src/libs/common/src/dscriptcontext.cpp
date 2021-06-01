/***************************************************************************
**
**  Implementation of the DScriptContext class
**
**  Creation date:  2017-07-31
**  Created by:     Matti Wennberg
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the base library
**
***************************************************************************/

//  PROJECT INCLUDES
//
#include    "dscriptcontext.h"

//  BASE INCLUDES
//
#include    "derror.h"

//  SYSTEM INCLUDES
//
#include    <fstream>

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )


/****************************************************************************/
/*! \class  DSensor  dscriptcontext.h
 *  \ingroup basecommon
 *  \brief  Class for creating a script context.
 *
 *  Parameters are passed in file with xml structure as an alternative to
 *  command line parameters. This helps to avoid problems with unicode characters,
 *  spaces and length limitations.
 */


//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! 
 *  Constructor
 */

DScriptContext::DScriptContext()
{
}


//----------------------------------------------------------------------------
/*! 
 *  Destructor
 */

DScriptContext::~DScriptContext()
{
}


//----------------------------------------------------------------------------
/*!
*  Adds a parameter to context
*  \param key the key to identify the set of values
*  \param value the value to be added
*/

void DScriptContext::addParameterValue( const std::string& key, const std::string& value )
{
    ERROR_F("DScriptContext::addParameterValue");

    for (unsigned int i = 0; i < m_Keys.size(); i++)
    {
        if (m_Keys[i] == key)
        {
            m_Values[i].push_back(value);
            break;
        }
    }

    m_Keys.push_back(key);
    m_Values.push_back(vector<string>());
    m_Values[m_Values.size() - 1].push_back(value);
}


//----------------------------------------------------------------------------
/*!
*  Write context to file
*  \param fileName path on disk
*  \return success state
*/

bool DScriptContext::write(const std::string& fileName) const
{
    ofstream out(fileName.c_str(), ofstream::binary);
    out << "<scriptcontext>";
    out << "<parameters>";
    for (unsigned int i = 0; i < m_Keys.size(); i++)
    {
        out << "<parameter>";
        out << "<key>" << m_Keys[i] << "</key>";
        out << "<values>";
        for (unsigned int j = 0; j < m_Values[i].size(); j++)
        {
            out << "<value><![CDATA[" << m_Values[i][j] << "]]></value>";
        }
        out << "</values>";
        out << "</parameter>";
    }
    out << "</parameters>";
    out << "</scriptcontext>";
    out.close();

    return true;
}
