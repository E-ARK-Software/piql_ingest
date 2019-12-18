#ifndef     DAPPLICATION_H
#define     DAPPLICATION_H

/*****************************************************************************
**  $Id: dapplication.h,v 1.1 2005/05/20 13:32:00 swhbt Exp $
**
**  Definition of DLFR DApplication application class
**
**  Creation date:  2004/05/24
**  Created by:     Han Timmerman
**
**  Project ID:     NO-04001-01S
**  Project name:   Piql DLFR
**  Project status: Create
**
**  Target(s):      Linux 2.4.x / GCC 3.0
**
**  Namespace:      DLFR
**
**  Dependencies:   -
**  Restrictions:   -
**  ToDo:           -
**
**  Copyright (c) 2004 ANT Software Development. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "dcommon.h"
#include    "dversion.h"

//  SYSTEM INCLUDES
//
#include    <vector>
#include    <string>

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )

//  FORWARD DECLARATIONS
//
class  DApplication;
class  DErrorMan;

//  GLOBAL
//
extern DApplication*  cvApp;


//============================================================================
//  CLASS:  DApplication

class  DApplication                     // Abstract
{

public:

    DApplication( int type, const char* argv0, const char* name=0 );
    DApplication( int type, const char* argv0, const DVersion& ver, const char* name=0 );
    virtual ~DApplication();

    const std::string&  name() const;
    const std::string&  directory() const;
    void                setVersion( const DVersion& ver );
    const DVersion&     release() const;
    std::string         releaseInfo() const;

    bool                setConfigPath( const std::string& path="", bool check=false );
    const std::string&  configPath() const;
    bool                setLogPath( const std::string& path="", bool renew=false );
    const std::string&  logPath() const;

    int                 maxProcessCount() const;

    int                 run( int maxproc=0, bool log=false );


protected:

    virtual bool        configure();
    virtual int         prepare();
    virtual int         execute() = 0;
    virtual void        finish();

    bool                makeUnique();
    int                 makeDaemon();


private:

    int                         m_AppType;
    std::string                 m_AppDirectory;
    std::string                 m_AppName;
    DVersion                    m_AppVersion;
    std::string                 m_ConfigPath;
    std::string                 m_LogPath;
    DErrorMan*                  m_ErrorManager;
    int                         m_MaxProcessCount;


private:    // DISABLE

    DApplication();
    DApplication( const DApplication& );
    DApplication&  operator = ( const DApplication& );

};


//----------------------------------------------------------------------------
//  INLINE: DApplication

inline const std::string&  DApplication::name() const
{
    return  m_AppName;
}


inline const std::string&  DApplication::directory() const
{
    return  m_AppDirectory;
}


inline const DVersion&  DApplication::release() const
{
    return  m_AppVersion;
}


inline const std::string&  DApplication::configPath() const
{
    return  m_ConfigPath;
}


inline const std::string&  DApplication::logPath() const
{
    return  m_LogPath;
}


inline int  DApplication::maxProcessCount() const
{
    return  m_MaxProcessCount;
}



//============================================================================

D_NAMESPACE_END


//
//===================================EOF======================================

#endif  /* DAPPLICATION_H */
