#ifndef CMD_UTIL_BASE_H
#define CMD_UTIL_BASE_H

/*****************************************************************************
**  $Id$
**
**  Base class for command line utils
**
**  Creation date:  2010-01-08
**  Created by:     Ole Liabo
**
**  Platform(s):    Intel, Linux 2.4.x, g++ 3.0
**                  Intel, Windows 2K/XP, VC++ 6.0
**
**  Dependencies:   -
**  Restrictions:   -
**  ToDo:           -
**
**  Copyright (c) 2010 Piql. All rights reserved.
**
**  This file is part of the service tools common library.
**
*****************************************************************************/

//  PROJECT INCLUDES
//

//  SYSTEM INCLUDES
//
#include <vector>
#include <string>

// CV INCLUDES
//
#include "dversion.h"
#include "dapplication.h"

//  NAMESPACE
//
D_NAMESPACE_FWD( TCLAP, Arg )
D_NAMESPACE_FWD( TCLAP, CmdLine )

//  FORWARD DECLARATIONS
//


//============================================================================
// CLASS: CmdUtilBase
class CmdUtilBase : public D_NAMESPACE::DApplication
{
public:
    typedef TCLAP::Arg CmdUtilArgument;
    typedef TCLAP::CmdLine CmdUtilParameters;
    typedef std::vector<CmdUtilArgument*> CmdUtilArguments;

public:
    CmdUtilBase(
        const std::string& applicationName, 
        const D_NAMESPACE::DVersion& version, 
        const std::string& helpText);
    virtual ~CmdUtilBase();

public:
    virtual int launch( int argc, char *argv[] );

protected:
    virtual int execute();
    bool addArgument( CmdUtilArgument& argument );
    bool addExclusiveArguments( CmdUtilArgument& a, CmdUtilArgument& b );
    bool addExclusiveArguments( CmdUtilArgument& a, CmdUtilArgument& b, CmdUtilArgument& c );
    bool addExclusiveArguments( CmdUtilArgument& a, CmdUtilArgument& b, CmdUtilArgument& c, CmdUtilArgument& d );
    bool addExclusiveArguments( CmdUtilArguments& arguments );
    virtual const std::string& helpText() const;
    virtual const std::string& applicationName() const;
    virtual const D_NAMESPACE::DVersion& version() const;
    virtual bool parseArguments(CmdUtilParameters& m_cmdLine) = 0;
    virtual int launchHandler() = 0;
    bool isLogLevelSet() const;
    
private:
    std::string m_helpText;
    std::string m_applicationName;
    D_NAMESPACE::DVersion m_version;
    CmdUtilParameters* m_cmdLine;
    int m_argc;
    char** m_argv;
    bool m_isLogLevelSet;
};

#endif /* CMD_UTIL_BASE_H*/
