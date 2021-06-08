/***************************************************************************
**  $Id: dpath.cpp,v 1.1 2004/04/06 20:09:02 swhbt Exp $
**
**  Implementation of the DLFR DPath path handling class
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
***************************************************************************/

//  PROJECT INCLUDES
//
#include    "dpath.h"
#include    "derror.h"
#include    "dstringtools.h"

//  SYSTEM INCLUDES
//
#if defined D_OS_WIN32
#   include   <direct.h>
#   include   <io.h>
#   include    <boost/locale/encoding_utf.hpp>
#else
#   include   <sys/stat.h>
#   include   <fnmatch.h>
#   include   <dirent.h>
#   include   <unistd.h>
#endif
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <errno.h>
#include    <fstream>

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )


/****************************************************************************/
/*! \class  DPath  dpath.h
 *  \ingroup basecommon
 *  \brief  This class provides path handling
 *
 *  This class provides path handling.
 *
 */

//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     DPath::DPath()
 *
 *  Default constructor. Constructs an empty path object.
 */

DPath::DPath()
{
    initClass();
}


//----------------------------------------------------------------------------
/*! \fn     DPath::DPath( const std::string& path )
 *  \param  path    Path spesification
 *
 *  Create a path object for specified path.
 *
 *  Note if path is: "./problem" or "problem", is 'problem' a file or a directory?
 *  The DPath is file greedy, meaning it will first ry to find a valid filename,
 *  then a directory.
 */

DPath::DPath( const std::string& path )
{
    initClass();
    setPath( path);
}


//----------------------------------------------------------------------------
/*! \fn     DPath::DPath( const std::string& dir, const std::string& fname )
 *  \param  dir     Directory specification
 *  \param  fname   Filename specification
 *
 *  Create a path object based on specified directory and filename.
 */

DPath::DPath( const std::string& dir, const std::string& fname )
{
    initClass();

    string  tmp = dir;

    appendDirSeparator( tmp );

    tmp += fname;

    setPath( tmp );
}


//----------------------------------------------------------------------------
/*! \fn     DPath::DPath( const DPath& po )
 *  \param  po      Other path object
 *
 *  Copy constructor. Create a deep copy of the specified path object.
 */

DPath::DPath( const DPath& po )
{
    initClass();

    setPath( po );
}


//----------------------------------------------------------------------------
/*! \fn     DPath&  DPath::operator = ( const std::string& path )
 *  \param  path    Path specification
 *  \return this path object
 *
 *  Assign from specified path specification.
 */

DPath&  DPath::operator = ( const std::string& path )
{
    setPath( path );

    return  *this;
}


//----------------------------------------------------------------------------
/*! \fn     DPath&  DPath::operator = ( const DPath& po )
 *  \param  po      Other path object
 *  \return this path object
 *
 *  Copy assignment operator. Assign other path object to this path object.
 */

DPath&  DPath::operator = ( const DPath& po )
{
    if (&po != this)
        setPath( po );

    return  *this;
}


//----------------------------------------------------------------------------
/*! 
 *  \param  po      Path specification
 *  \return \a true if po is equal to this otherwise return \a false
 *
 *  Compare two path pobjects.
 */

bool DPath::operator == ( const DPath& po ) const
{
    return 
        m_Path       == po.m_Path &&
        m_Directory  == po.m_Directory &&
        m_Basename   == po.m_Basename &&
        m_Extension  == po.m_Extension &&
        m_IsValid    == po.m_IsValid &&
        m_IsAbsPath  == po.m_IsAbsPath;
}


//----------------------------------------------------------------------------
/*! \fn     bool    DPath::setPath( const std::string& path )
 *  \param  path    Path specification
 *  \return \a true if resulting path is valid otherwise return \a false
 *
 *  Set path from specified path and return \a true if the resulting path is
 *  a valid path, otherwise return \a false.
 *
 *  The existance of the path is not verified.
 */

bool    DPath::setPath( const std::string& path )
{
    if (path.empty())
    {
        initClass();
        return  false;
    }

    if (path == m_Path)
    {
        return  m_IsValid;
    }

    initClass();

    m_Path = path;
    m_IsValid = disassemble();

    return  m_IsValid;
}


//----------------------------------------------------------------------------
/*! \fn     bool    DPath::setPath( const std::string& dir, const std::string& fname )
 *  \param  dir     Directory specification
 *  \param  fname   Filename specification
 *  \return \a true if resulting path is valid otherwise return \a false
 *
 *  Set path from specified directory and filename and return \a true if the
 *  resulting path is a valid path, otherwise return \a false.
 *
 *  The existance of the resulting path is not verified.
 */

bool    DPath::setPath( const std::string& dir, const std::string& fname )
{
    string  tmp = dir;

    appendDirSeparator( tmp );

    tmp += fname;

    return  setPath( tmp );
}


//----------------------------------------------------------------------------
/*! \fn     bool    DPath::setPath( const DPath& po )
 *  \param  po      Other path object
 *  \return \a true if resulting path is valid otherwise return \a false
 *
 *  Set path from information of other path object.
 */

bool    DPath::setPath( const DPath& po )
{
    if (&po != this)
        return  setPath( po.path() );

    return  isValid();
}

bool    DPath::appendDir( const std::string& dir )
{
    string tmp = dir;
    appendDirSeparator( tmp );
    setPath( directory(), tmp );

    return  isValid();
}

bool    DPath::appendFilename( const std::string& fname )
{
    setPath( directory(), fname );

    return  isValid();
}

//----------------------------------------------------------------------------
/*! \fn     std::string  DPath::directory() const
 *  \return current directory specification
 *
 *  Return current directory specification.
 */


//----------------------------------------------------------------------------
/*! \fn     std::string  DPath::basename() const
 *  \return current basename specification
 *
 *  Return current basename specification. This basename is the base filename
 *  without the file extension.
 */


//----------------------------------------------------------------------------
/*! \fn     std::string  DPath::extension() const
 *  \return current file extension specification
 *
 *  Return current filename extension specification. The extension does not
 *  include the '.', so for 'file.pdf' extension() will return 'pdf'.
 */


//----------------------------------------------------------------------------
/*! \fn     std::string  DPath::filename() const
 *  \return current filename specification
 *
 *  Return current filename specification.
 */

std::string  DPath::filename() const
{
    std::string  ret = basename();

    if ( !extension().empty() )
    {
        ret += "." + extension();
    }

    return  ret;
}


//----------------------------------------------------------------------------
/*! \fn     std::string  DPath::path() const
 *  \return current path
 *
 *  Return current path.
 */

std::string  DPath::path() const
{
    std::string  ret = directory() + filename();

    return  ret;
}


//----------------------------------------------------------------------------
/*! \fn     std::string  DPath::pathList() const
*  \return current path as one directory/file per element
*
*  Return current path.
*/

std::vector<std::string> DPath::pathList() const
{
    std::vector<std::string> ret;
    DStringTools::stringSplit( directory(), ret, dirSeparator() );
    ret.push_back( filename() );

    return ret;
}


//----------------------------------------------------------------------------
/*! \fn     bool  DPath::isFile() const
 *  \return is file?
 *
 *  Return true if item is a file.
 */

bool  DPath::isFile() const
{
    return  !filename().empty();
}


//----------------------------------------------------------------------------
/*! \fn     bool  DPath::isDirectory() const
 *  \return is directory?
 *
 *  Return true if item is a directory.
 */

bool  DPath::isDirectory() const
{
    return ( filename().empty() && !directory().empty() );
}


//============================================================================
//  S T A T I C   I N T E R F A C E
//============================================================================

bool DPath::isExistingPath( const DPath& path )
{
    return isExisting( path.path() );
}

bool DPath::isExistingDir( const DPath& path )
{
    return isExisting( path.directory() );
}

//----------------------------------------------------------------------------
/*! \param path Directory to create
 *  \param recursive Recursively create all subdirs
 *  \return Success state
 *
 *  Create a directory. Return true also if path exists.
 */

bool  DPath::makeDir( const DPath& path, bool recursive/*=false*/ )
{
    ERROR_F( "DPath::makeDir" );

    string pathOnly = path.directory();
    size_t pos = pathOnly.find_first_of( "\\/" );
    while ( pos != string::npos )
    {
        string createPath;
        if ( recursive )
        {
            createPath = pathOnly.substr( 0, pos + 1 );
            pos = pathOnly.find_first_of( "\\/", pos + 1 );
        }
        else
        {
            createPath = pathOnly;
            pos = string::npos;
        }

        if ( !isExisting( createPath ) )
        {
            int res = 1;
#if defined (D_OS_WIN32)
            wstring multiBytePath;
            if ( !DStringTools::toWstring(createPath, multiBytePath) )
            {
                error << ERRerror << "Failed to convert string" << endl;
                return false;
            }
            res = _wmkdir( multiBytePath.c_str() );
#else    
            res = mkdir( createPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
#endif

            if (res != 0)
            {
                error << ERRwarning << "Failed to create dir:" << pathOnly.c_str() << std::endl;
                return false;
            }
        }
    }

    return true;
}


//----------------------------------------------------------------------------
/*! 
 *  File handler class used by the scanDir function.
 */

class DScanDirHandler: public DFileHandler
{
public:
    
    DScanDirHandler( vector<string>& files, bool includeDirs, bool includeFiles, bool fullPath ) 
        : m_Files( files ), m_IncludeDirs( includeDirs ), m_IncludeFiles( includeFiles), m_FullPath( fullPath ) {};

    virtual bool handle( const DPath& path, const char* filename, bool isDir )
    {
        if ( !m_FullPath )
        {
            if ( (isDir && m_IncludeDirs) || (!isDir && m_IncludeFiles) )
            {
                m_Files.push_back( filename );
            }
        }
        else
        {
            if ( isDir && m_IncludeDirs )
            {
                DPath cpath( path );
                cpath.appendDir( filename );
                m_Files.push_back( cpath.path() );
            }
            else if ( m_IncludeFiles && !isDir )
            {
                DPath cpath( path );
                cpath.appendFilename( filename );
                m_Files.push_back( cpath.path() );
            }
        }
        
        return true;
    }

private:

    vector<string>& m_Files;
    bool m_IncludeDirs;
    bool m_IncludeFiles;
    bool m_FullPath;
};


//----------------------------------------------------------------------------
/*! \param path        Directory to scan
 *  \param wildcard    Shell wildcard pattern
 *  \param files[out]  Vector to be populated with filenames found, excluding path if fullPath is false.
 *  \param ignoreCase  Ignore filename case when matching
 *  \param recursive   Include files in matching subdirectories
 *  \param includeDirs Include directories in search
 *  \param includeFiles Include files in search.
 *  \param fullPath    Include full path for items found.
 *  \return Success state
 *
 *  Scan a directory for files/subdirectories matching wildcard. One or both of includeDirs and includeFiles must be true;
 */

bool  DPath::scanDir( const DPath& path, const std::string& wildcard, std::vector<std::string>& files, bool ignoreCase, bool recursive, bool includeDirs, bool includeFiles /*= true*/, bool fullPath /*= false*/ )
{
    ERROR_F( "DPath::scanDir" );

    files.clear();

    if ( !(includeDirs || includeFiles) )
    {
        error << ERRerror << "Both includeDirs and includeFiles are false" << endl;
        return false;
    }

    DScanDirHandler fileHandler( files, includeDirs, includeFiles, fullPath );
    return enumerateFiles( 
        path, 
        wildcard, 
        fileHandler, 
        recursive, 
        ignoreCase );
}


//----------------------------------------------------------------------------
/*! \param temp        Temporary filename created by the function        
 *  \param dir         Directory where the temp file should be created. System temp dir has precedence if existing.
 *  \param prefix      Filename prefix
 *  \return Success state
 *
 *  Create temp filename.
 */

bool DPath::getTempFilename( DPath& temp, const DPath& dir, const std::string& prefix )
{
    ERROR_F( "DPath::getTempFilename" );

#if defined(D_OS_WIN32) 
    char* tn = _tempnam( dir.directory().c_str(), prefix.c_str() );
    if ( tn == NULL )
    {
        error << ERRinfo << "Failed to create temp file name in folder: '" << dir << "'" << endl;
        return false;
    }
#else
    ///\todo Use new/delete instead of malloc/free here
    char* tn = (char*)malloc( PATH_MAX );
    strcpy( tn, dir.directory().c_str() );
    if ( strlen(tn) && tn[strlen(tn)-1] != '/' )
    {
        strcat( tn, "/" );
    }
    strcat( tn, prefix.c_str() );
    strcat( tn, "XXXXXX" );

    errno = 0;
    int file = mkstemp(tn);
    if ( file == -1 )
    {
        error << ERRinfo << "Failed to create temp file name " << tn <<  "in folder: '" << dir << "'" << " error=" << strerror(errno) << endl;
        free( tn );
        return false;
    }
    unlink( tn );
    close( file );
#endif


    if ( !temp.setPath( tn ) )
    {
        error << ERRinfo << "Failed to create temp file name in folder: '" << dir << "'" << endl;
        free( tn );
        return false;
    }

    free( tn );
    return true;
}


//----------------------------------------------------------------------------
/*! \param temp        Temporary filename created by the function        
 *  \param prefix      Filename prefix
 *  \return Success state
 *
 *  Create temp filename in dir returned by systemTempDir().
 */

bool DPath::getTempFilename( DPath& temp, const std::string& prefix )
{
    DPath tempDir;
    if ( !DPath::systemTempDir( tempDir ) )
    {
        return false;
    }

    return DPath::getTempFilename( temp, tempDir, prefix );
}


//----------------------------------------------------------------------------
/*! \param temp        Temporary dirname created by the function        
 *  \param dir         Directory where the temp dir should be below
 *  \param prefix      Dirname prefix
 *  \return Success state
 *
 *  Create temp dirname.
 */

bool DPath::getTempDir( DPath& temp, const DPath& dir, const std::string& prefix )
{
    DPath tempFilename;
    if ( getTempFilename( tempFilename, dir, prefix ) )
    {
        temp.setPath( tempFilename.directory(), string(".") );
        temp.appendDir( tempFilename.filename() );

        return temp.isValid();
    }
    return false;
}


//----------------------------------------------------------------------------
/*! \param tempDir   System temp dir location
 *  \return Success state
 *
 *  Get system temp dir path.
 */

bool DPath::systemTempDir( DPath& tempDir )
{
#if defined(D_OS_WIN32) 
    const int nameLen = MAX_PATH + 1;
    char name[nameLen];
    int ret = GetTempPathA( nameLen, name );
    
    tempDir = DPath( name );
    
    return ret != 0;
#else
    char * name = getenv("TMPDIR");
    if ( name == NULL )
    {
        tempDir = DPath( "/tmp/" );
    }
    else
    {
        tempDir = DPath( name, "." );
    }
    return true;
#endif
}


//----------------------------------------------------------------------------
/*! 
 *  Filehandler used by the deleteDir function.
 */

class DDeleteFileHandler : public DFileHandler
{
public:

    virtual bool handle( const DPath& path, const char* filename, bool isDir )
    {
        ERROR_F( "DDeleteFileHandler::handle" );

        if ( isDir )
        {
            DPath cpath( path );
            cpath.appendDir( filename );
            error << ERRdebug << "Deleting dir: " << cpath << endl;
            return DPath::deleteDir( cpath );
        }
        else
        {
            DPath cpath( path );
            cpath.appendFilename( filename );
            error << ERRdebug << "Deleting file: " << cpath << endl;
            return DPath::deleteFile( cpath );
        }
    }
};


//----------------------------------------------------------------------------
/*! \param dir         Directory to delete
 *  \param removeFiles If true all files and subdirectories are deleted
 *  \return Success state
 *
 *  Delete direcory and optionally delete all files and directories under the 
 *  directory as well.
 */

bool DPath::deleteDir( const DPath& dir, bool removeFiles/*=false*/ )
{
    ERROR_F( "DPath::deleteDir" );

    if ( removeFiles )
    {
        if ( !cleanDir( dir ) )
        {
            return false;
        }
    }

#if defined(D_OS_WIN32) 
    wstring multiBytePath;
    if ( !DStringTools::toWstring(dir.directory(), multiBytePath) )
    {
        error << ERRerror << "Failed to convert string" << endl;
        return false;
    }
    int res = _wrmdir( multiBytePath.c_str() );
#else
    int res = rmdir( dir.directory().c_str() );
#endif

    if ( res != 0 )
    {
        error << ERRinfo << "Failed to remove dir: '" << dir << "'" << endl;
        return false;
    }
    else
        error << ERRinfo << "Removed dir: '" << dir << "'" << endl;

    return res == 0;
}


//----------------------------------------------------------------------------
/*! \param dir         Directory to delete
 *  \return Success state
 *
 *  Delete all content in direcory including subdirs.
 */

bool DPath::cleanDir( const DPath& dir )
{
    ERROR_F( "DPath::cleanDir" );

    DDeleteFileHandler deleteFileHandler;
    if ( !enumerateFiles( 
        dir, 
        "*", 
        deleteFileHandler, 
        true, 
        true ) )
    {
        error << ERRinfo << "Failed to delete files in folder: '" << dir << "'" << endl;
        return false;
    }
    return true;
}



//----------------------------------------------------------------------------
/*! \param file        File to delete
 *  \return Success state
 *
 *  Delete file.
 */

bool DPath::deleteFile( const DPath& file )
{
    ERROR_F( "DPath::deleteFile" );

#if defined(D_OS_WIN32)
    wstring wideString;
    if ( !DStringTools::toWstring( file.path(), wideString ) )
    {
        error << ERRwarning << "String conversion failed for: '" << file.path() << "'" << endl;
        return false;
    }
    int res = DeleteFileW( wideString.c_str() ) ? 0 : 1;
#else
    int res = remove( file.path().c_str() );
#endif

    if ( res != 0 )
    {
#if defined(D_OS_WIN32)
        error << ERRinfo << "Failed to remove file: '" << file << "' error=" << GetLastError() << endl;
#else
        error << ERRinfo << "Failed to remove file: '" << file << "'" << endl;
#endif
    }

    return res == 0;
}


//----------------------------------------------------------------------------
/*! \param path        Directory to scan
 *  \param wildcard    Filename wildcard
 *  \param fileHandler Callback, will be called for each file or directory matching wildcard.
 *  \param recursive   Include files in matching subdirectories
 *  \param ignoreCase  Ignore filename case when matching
 *  \return Success state
 *
 *  Scan a directory for files matching wildcard.
 */

bool DPath::enumerateFiles( 
    const DPath& path, 
    const std::string& wildcard, 
    DFileHandler& fileHandler, 
    bool recursive, 
    bool ignoreCase )
{
    ERROR_F( "DPath::enumerateFiles" );

    if ( !path.isValid() )
    {
        error << ERRerror << "Invalid path" << endl;
        return false;
    }

#if defined (D_OS_WIN32) 
    string target = path.directory() + wildcard;
    struct _wfinddata_t c_file;
    intptr_t hFile;

    wstring targetMultiByte;
    if ( !DStringTools::toWstring(target, targetMultiByte) )
    {
        error << ERRerror << "Failed to convert string" << endl;
        return false;
    }

    // Find first file
    if( (hFile = _wfindfirst( targetMultiByte.c_str(), &c_file )) == -1L )
    {
        if (errno == EINVAL)
        {
            // Invalid filename specification
            error << ERRerror << "Invalid file name spec" << endl;
            return false;
        }
        // No files found, but not an error
        return true;
    }
    else
    {
        string filename;
        if (!DStringTools::toUtf8(c_file.name, filename))
        {
            error << ERRerror << "Failed to convert filename" << endl;
            return false;
        }

        // Handle first file
        bool isDir = (c_file.attrib & _A_SUBDIR) != 0;

        if ( wcscmp(c_file.name, L".") != 0 && wcscmp(c_file.name, L"..") != 0 )
        {
            if ( isDir && recursive )
            {
                DPath subdir( path.path(), filename );
                bool ok = enumerateFiles( subdir, wildcard, fileHandler, recursive, ignoreCase );
                if ( !ok )
                {
                    _findclose( hFile );
                    return false;
                }
            }
            if ( !fileHandler.handle( path, filename.c_str(), isDir ) )
            {
                _findclose( hFile );
                return false;
            }
        }

        // Find the rest of the files
        while( _wfindnext( hFile, &c_file ) == 0 )
        {
            if ( !DStringTools::toUtf8(c_file.name, filename) )
            {
                error << ERRerror << "Failed to convert filename" << endl;
                return false;
            }

            if ( wcscmp(c_file.name, L".") != 0 && wcscmp(c_file.name, L"..") != 0 )
            {
                isDir = (c_file.attrib & _A_SUBDIR) != 0;
                if ( isDir && recursive )
                {
                    DPath subdir( path.directory() );
                    subdir.appendDir( filename );
                    bool ok = enumerateFiles( subdir, wildcard, fileHandler, recursive, ignoreCase );
                    if ( !ok )
                    {
                        _findclose( hFile );
                        return false;
                    }
                }

                if ( !fileHandler.handle( path, filename.c_str(), isDir ) )
                {
                    _findclose( hFile );
                    return false;
                }
            }
        }

       _findclose( hFile );
   }
   return true;
#else
    // Set up system-specific case-ignore switch for fnmatch() call.
    #if defined( FNM_CASEFOLD )
    #define DPATH_IGNORECASE FNM_CASEFOLD
    #elif defined( FNM_IGNORECASE )
    #define DPATH_IGNORECASE FNM_IGNORECASE
    #endif

    int matchFlags = 0;
    if ( ignoreCase )
    {
        matchFlags = DPATH_IGNORECASE;
    }

    DIR *dir;
    struct dirent *dirp;

    dir = opendir(path.directory().c_str());

    if ( dir == NULL)
    {
        error << ERRinfo << "failed to open directory: '" << path.directory() << "'" << endl;
        return false;
    }

    while ( ( dirp = readdir( dir ) ) != NULL )
    {
        if ( strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0 )
        {
            continue;
        }

        std::string fileName = dirp->d_name;
        if ( fnmatch( wildcard.c_str(), fileName.c_str(), matchFlags ) == 0 )
        {
            DPath currentPath( path.directory(), fileName );
            error << ERRdebug << "Found: " << currentPath << endl;
            
            struct stat statbuf;
            if ( stat( currentPath.path().c_str(), &statbuf) == -1 )
            {
                closedir( dir );
                error << ERRerror << "failed to get attributes for: '" << dirp->d_name << "'" << endl;
                return false;
            }

            bool isDir = S_ISDIR( statbuf.st_mode );

            if ( recursive && isDir )
            {
                DPath subdir( path.directory() );
                subdir.appendDir( dirp->d_name );
                bool ok = enumerateFiles( subdir, wildcard, fileHandler, recursive, ignoreCase );
                if ( !ok )
                {
                    error << ERRerror << "failed to enumerate files in: '" << subdir << "'" << endl;

                    closedir( dir );
                    return false;
                }
            }

            if ( !fileHandler.handle( path, dirp->d_name, isDir ) )
            {
                error << ERRerror << "file handler failed for: '" << path << "'" << endl;
                closedir( dir );
                return false;
            }
        }
    }
    closedir( dir );

    return true;
#endif
}

bool DPath::rename( const DPath& oldPath, const DPath& newPath )
{
    ERROR_F( "DPath::rename" );

    int res = ::rename( oldPath.path().c_str(), newPath.path().c_str() );

    if ( res != 0 )
    {
        // TODO: if errno = EXDEV (Invalid cross-device link), use copy and delete.
        int errnum = errno;
        error << ERRinfo << "Rename failed. old='" << oldPath << "' new='" << newPath << "'"
              << ". Reason: " << strerror(errnum) << " (" << errnum << ")" << endl;
    }
    return res == 0;
}

bool DPath::copyFile( const DPath& source, const DPath& destination )
{
    ERROR_F( "DPath::copy" );

    std::ifstream  src( source.path().c_str(), std::ios::binary );
    std::ofstream  dst( destination.path().c_str(), std::ios::binary );

    dst << src.rdbuf();

    return dst.good();
}


//============================================================================
//  P R I V A T E   I N T E R F A C E
//============================================================================

void    DPath::initClass()
{
    m_IsValid = false;
    m_IsAbsPath = false;
}


bool    DPath::disassemble()
{
#if defined (D_OS_UNIX)
    m_IsAbsPath = false;
    m_Extension = "";

    if (m_Path.empty())
    {
        return  false;
    }

    size_t  len = m_Path.length();
    size_t  pos = 0;

    if (m_Path[0] == '/')
    {
        m_IsAbsPath = true;
    }
        
    pos = m_Path.rfind( '/' );
    if (pos == string::npos)
    {
        m_Directory = "./";
        m_Basename = m_Path;
    }
    else if (pos == len)
    {
        m_Directory = m_Path.substr( 0, pos );
        m_Basename = "";
    }
    else
    {
        m_Directory = m_Path.substr( 0, pos + 1 );
        m_Basename = m_Path.substr( pos + 1 );
    }

    len = m_Basename.length();
    if (len == 0)
    {
        return  (!m_Directory.empty());
    }
        
    pos = m_Basename.rfind( '.' );
    if ((pos != string::npos) && (pos < len))
    {
        m_Extension = m_Basename.substr( pos + 1 );
        m_Basename.erase( pos, len - pos );
    }

    return  true;
#elif defined (D_OS_WIN32)

    if (m_Path.empty())
    {
        return  false;
    }
        
    // The path could be a com port, special case
    if ( m_Path.length() == 5 && m_Path[4] == ':' )
    {
        if ( (m_Path[0] == 'c' || m_Path[0] == 'C') && 
             (m_Path[1] == 'o' || m_Path[1] == 'O') &&
             (m_Path[2] == 'm' || m_Path[2] == 'M') )
        {
            m_Directory = m_Path;
            m_IsAbsPath = true;
            m_Extension = "";
            m_Basename = "";     
            return true;                   
        }
    }

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    _splitpath( m_Path.c_str(), drive, dir, fname, ext );

    m_IsAbsPath = false;
    m_Extension = "";

    if (m_Path.empty())
    {
        return  false;
    }
        
    if ( strlen(drive) != 0 )
    {
        m_IsAbsPath = true;
    }
        
    m_Directory = drive;

    if ( strlen(dir) )
    {
        m_Directory.append( dir );
    }
    else
    {
        m_Directory.append( ".\\" );
    }
    m_Basename = fname;

    if ( strlen(ext) )
    {
        m_Extension = ext + 1;
    }

    return  true;
#else
#error Unsupported OS.
#endif
}

void DPath::appendDirSeparator( std::string& tmp )
{
    if (!tmp.empty() && tmp.substr(tmp.length()-1) != dirSeparator())
    {
        tmp += dirSeparator();
    }
}

std::string DPath::dirSeparator()
{
#if defined (D_OS_LINUX)
    return "/";
#else
    return "\\";
#endif
}

std::string DPath::normalizeSeparators( const std::string& rawString )
{
    std::string unwantedSeparator = 
#if defined (D_OS_LINUX)
        "\\";
#else
        "/";
#endif

    std::vector<std::string> ret;
    DStringTools::stringSplit( rawString, ret, unwantedSeparator );

    std::stringstream result;
    for ( size_t i = 0; i < ret.size(); i++ )
    {
        if ( ret.at( i ).length() > 0 )
        {
            result << ret.at( i );
            if ( i < ret.size() - 1 ) result << dirSeparator();
        }
    }

    return result.str();
}

bool DPath::isExisting( const std::string& path )
{
    ERROR_F( "DPath::isExisting" );

#if defined ( D_OS_UNIX )
    while (access( path.c_str(), F_OK ))
#elif defined ( D_OS_WIN32 )
    wstring multiBytePath;
    if ( !DStringTools::toWstring(path, multiBytePath) )
    {
        error << ERRerror << "Failed to convert string" << endl;
        return false;
    }
    while (_waccess(multiBytePath.c_str(), 0))
#else
#error
#endif // D_OS_UNIX
    {
        if (errno != EINTR)
        {
            return  false;
        }
    }

    return  true;
}

//============================================================================
//  D E B U G   I N T E R F A C E
//============================================================================

DError& D_NAMESPACE::operator<<( DError& error, const DPath& path )
{
    error << path.path();
    return error;
}


/********************************** EOF *************************************/
