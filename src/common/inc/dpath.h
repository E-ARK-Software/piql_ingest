#ifndef     DPATH_H
#define     DPATH_H

/*****************************************************************************
**  $Id: dpath.h,v 1.1 2004/04/06 20:04:17 swhbt Exp $
**
**  Definition of DLFR DPath path handling class
**
**  Creation date:  2004/04/06
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
#include    "derror.h"

//  SYSTEM INCLUDES
//
#include    <string>
#include    <vector>

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )

//  FORWARD DECLERATIONS
//
class DFileHandler;

//============================================================================
//  CLASS:  DPath

class  DPath
{

public:

    DPath();
    DPath( const std::string& path );
    DPath( const std::string& dir, const std::string& fname );
    DPath( const DPath& po );
   ~DPath()         {}

    DPath&          operator = ( const std::string& path );
    DPath&          operator = ( const DPath& po );
    bool            operator == ( const DPath& po ) const;

    bool            setPath( const std::string& path );
    bool            setPath( const std::string& dir, const std::string& fname );
    bool            setPath( const DPath& po );
    bool            appendDir( const std::string& dir );
    bool            appendFilename( const std::string& fname );

    std::string              directory() const;
    std::string              basename() const;
    std::string              extension() const;
    std::string              filename() const;
    std::string              path() const;
    std::vector<std::string> pathList() const;

    bool            isValid() const;
    bool            isAbsolute() const;
    bool            isFile() const;
    bool            isDirectory() const;

public:

    static bool        isExistingPath( const DPath& path );
    static bool        isExistingDir( const DPath& path );
    static bool        makeDir( const DPath& path, bool recursive=false );
    static bool        scanDir( const DPath& path, const std::string& wildcard, std::vector<std::string>& files, bool ignoreCase, bool recursive, bool includeDirs, bool includeFiles = true, bool fullPath = false );
    static bool        getTempFilename( DPath& temp, const DPath& dir, const std::string& prefix );
    static bool        getTempFilename( DPath& temp, const std::string& prefix );
    static bool        getTempDir( DPath& temp, const DPath& dir, const std::string& prefix );
    static bool        systemTempDir( DPath& tempDir );    
    static bool        deleteDir( const DPath& dir, bool removeFiles=false );
    static bool        cleanDir( const DPath& dir );
    static bool        deleteFile( const DPath& file );
    static bool        enumerateFiles( const DPath& path, const std::string& wildcard, DFileHandler& fileHandler, bool recursive, bool ignoreCase );
    static bool        rename( const DPath& oldPath, const DPath& newPath );
    static bool        copyFile( const DPath& source, const DPath& destination );
    static void        appendDirSeparator( std::string& str );
    static std::string dirSeparator();
    static std::string normalizeSeparators( const std::string& rawString );

private:

    void               initClass();
    bool               disassemble();

private: 

    static bool        isExisting( const std::string& path );

private:

    std::string     m_Path;
    std::string     m_Directory;
    std::string     m_Basename;
    std::string     m_Extension;
    bool            m_IsValid;
    bool            m_IsAbsPath;

};

DError& operator<<( DError& err, const DPath& path );


//============================================================================
//  CLASS:  DFileHandler

class  DFileHandler
{
public:
    virtual ~DFileHandler() {};
    virtual bool handle( const DPath& path, const char* filename, bool isDir ) = 0;
};

//----------------------------------------------------------------------------
//  INLINE: DPath

inline bool  DPath::isValid() const
{
    return  m_IsValid;
}


inline bool  DPath::isAbsolute() const
{
    return  m_IsAbsPath;
}


inline std::string  DPath::directory() const
{
    return  m_Directory;
}


inline std::string  DPath::basename() const
{
    return  m_Basename;
}


inline std::string  DPath::extension() const
{
    return  m_Extension;
}



//============================================================================

D_NAMESPACE_END


//
//===================================EOF======================================

#endif  /* DPATH_H */
