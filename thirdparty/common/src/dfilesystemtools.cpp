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
#include    "dfilesystemtools.h"
#include    "dsystemtools.h"
#include    "dstringtools.h"
#include    "derror.h"

//  SYSTEM INCLUDES
//
#ifdef D_OS_LINUX
#include    <sys/statvfs.h>
#include    <unistd.h>
#endif
#include    <sys/stat.h>
#include    <fcntl.h>
#include    <fstream>
#include    <errno.h>
#include    <string.h>
#ifdef D_OS_WIN32
#include    <io.h>
#include    <boost/locale/encoding_utf.hpp>
#endif

//  NAMESPACE
//
D_NAMESPACE_USING(D_NAMESPACE)
D_NAMESPACE_USING(std)

/****************************************************************************/
/*! \class  DFileSystemTools  dfilesystemtools.h
 *  \ingroup basecommon
 *  \brief  This class provides basic file system tools.
 *
 */


//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     bool  DFileSystemTools::isExistingPath( const std::string& path )
 *  \param  path    Path to be checked
 *  \return \a true if path exists otherwise return \a false
 *
 *  Return \a true if specified path is existing otherwise return \a false.
 */

 bool  DFileSystemTools::isExistingPath(const std::string& path)
{
#if defined ( D_OS_UNIX )
    while ( access( path.c_str(), F_OK ) )
#elif defined ( D_OS_WIN32 )
    wstring multiBytePath;
    if ( !DStringTools::toWstring( path, multiBytePath ) )
    {
        ERROR_F( "DFileSystemTools::isExistingPath" );
        error << ERRerror << "Failed to convert string" << endl;
        return false;
    }
    while (_waccess( multiBytePath.c_str(), 0))
#endif // D_OS_UNIX
    {
        if ( errno != EINTR )
        {
            return  false;
        }
    }

    return  true;
}


//----------------------------------------------------------------------------
/*! \fn     bool  DFileSystemTools::isReadablePath( const std::string& path )
 *  \param  path    Path to be checked
 *  \return \a true if path is readable otherwise return \a false
 *
 *  Return \a true if specified path is readable otherwise return \a false.
 */

bool  DFileSystemTools::isReadablePath(const std::string& path)
{
#if defined ( D_OS_UNIX )
    while ( access( path.c_str(), R_OK ) )
#elif defined ( D_OS_WIN32 )
    wstring multiBytePath;
    if ( !DStringTools::toWstring( path, multiBytePath ) )
    {
        ERROR_F( "DFileSystemTools::isReadablePath" );
        error << ERRerror << "Failed to convert string" << endl;
        return false;
    }
    while ( _waccess( multiBytePath.c_str(), 4 ) )
#endif // D_OS_UNIX
    {
        if ( errno != EINTR )
        {
            return  false;
        }
    }

    return  true;
}


//----------------------------------------------------------------------------
/*! \fn     bool  DFileSystemTools::isExecutableFile( const std::string& fn )
 *  \param  fn      Filename to be checked
 *  \return \a true if file is executable otherwise return \a false
 *
 *  Return \a true if specified file is executable otherwise return \a false.
 */

#if defined ( D_OS_UNIX )

bool  DFileSystemTools::isExecutableFile( const std::string& fn )
{
    while ( access( fn.c_str(), X_OK ) )
    {
        if ( errno != EINTR )
        {
            return  false;
        }
    }

    return  true;
}

#elif defined ( D_OS_WIN32 )

bool  DFileSystemTools::isExecutableFile(const std::string& /*fn*/)
{
    return  true;
}

#endif // D_OS_UNIX 


static void closeComparedFiles(int fileDescriptorA, int fileDescriptorB)
{
#if defined (D_OS_WIN32)
    if (fileDescriptorA != -1)
    {
        _close(fileDescriptorA);
    }
    if (fileDescriptorB != -1)
    {
        _close(fileDescriptorB);
    }
#else
    if (fileDescriptorA != -1)
    {
        close(fileDescriptorA);
    }
    if (fileDescriptorB != -1)
    {
        close(fileDescriptorB);
    }
#endif
}


//----------------------------------------------------------------------------
/*! 
 *  \param fileA Filename of first file
 *  \param fileB Filename of secound file
 *  \return true if the files are equal
 *
 *  Does a crude memcmp compare of the two input files.
 */

bool DFileSystemTools::compareFiles(const std::string& fileA, const std::string& fileB)
{
    ERROR_F("DFileSystemTools::compareFiles");

    int fileDescriptorA, fileDescriptorB;

#if defined (D_OS_WIN32)
    fileDescriptorA = _open(fileA.c_str(), _O_RDONLY);
    fileDescriptorB = _open(fileB.c_str(), _O_RDONLY);
#elif defined (D_OS_X)
    fileDescriptorA = open(fileA.c_str(), D_OM_RDONLY);
    fileDescriptorB = open(fileB.c_str(), D_OM_RDONLY);
#else
    fileDescriptorA = open(fileA.c_str(), O_RDONLY);
    fileDescriptorB = open(fileB.c_str(), O_RDONLY);
#endif

    if (fileDescriptorA == -1 || fileDescriptorB == -1)
    {
        closeComparedFiles(fileDescriptorA, fileDescriptorB);
        error << ERRerror << "Failed to open files" << endl;
        return false;
    }

    int returnValueA, returnValueB;

#if defined (D_OS_WIN32)
    struct _stati64 statBufferA, statBufferB;
    returnValueA = _fstati64(fileDescriptorA, &statBufferA);
    returnValueB = _fstati64(fileDescriptorB, &statBufferB);
#else
    struct stat statBufferA, statBufferB;
    returnValueA = fstat(fileDescriptorA, &statBufferA);
    returnValueB = fstat(fileDescriptorB, &statBufferB);
#endif

    if (returnValueA != returnValueB && statBufferA.st_size != statBufferB.st_size)
    {
        closeComparedFiles(fileDescriptorA, fileDescriptorB);
        return false;
    }

    // Read and compare in 64K chunks
    const size_t compareSize = 0xffff;
    char bufferA[compareSize];
    char bufferB[compareSize];

    UINT64 fileSize = statBufferA.st_size;

    while ( fileSize )
    {
        // Read
        size_t readSize = fileSize < compareSize ? (size_t)fileSize : compareSize;

#if defined (D_OS_WIN32)
        size_t aRead = _read(fileDescriptorA, bufferA, readSize);
        size_t bRead = _read(fileDescriptorB, bufferB, readSize);
#else
        size_t aRead = read(fileDescriptorA, bufferA, readSize);
        size_t bRead = read(fileDescriptorB, bufferB, readSize);
#endif

        if ( aRead != bRead )
        {
            closeComparedFiles(fileDescriptorA, fileDescriptorB);
            return false;
        }

        if ( memcmp( bufferA, bufferB, readSize ) != 0 )
        {
            closeComparedFiles(fileDescriptorA, fileDescriptorB);
            return false;
        }

        fileSize -= readSize;
    }

    closeComparedFiles(fileDescriptorA, fileDescriptorB);

    return true;
}


//----------------------------------------------------------------------------
/*! 
 *  Copy folder from source to destination.
 */

bool DFileSystemTools::copyFolder(const std::string& source, const std::string& destination)
{
#if defined (D_OS_LINUX)
    string command = "cp -R \"" + source + "\" \"" + destination + "\"";
#else
    string command = "copy \"" + source + "\" \"" + destination + "\"";
#endif
    return DSystemTools::doShellCommand( command ) == 0;
}


//----------------------------------------------------------------------------
/*! 
 *  Open source file and read content into string.
 */

bool DFileSystemTools::readFileIntoString(std::string& text, const std::string& source)
{
#if defined ( D_OS_WIN32 )
    wstring multibytePath;
    if ( !DStringTools::toWstring( source, multibytePath ) )
    {
        ERROR_F( "DFileSystemTools::readFileIntoString" );
        error << ERRwarning << "Could not convert string: '" << source << "' to multibyte." << endl;
        return false;
    }
    ifstream in( multibytePath.c_str() );
#else
    ifstream in( source.c_str() );
#endif
    if ( !in )
    {
        return false;
    }
    
    std::istreambuf_iterator<char> eos;
    text = string( std::istreambuf_iterator<char>(in), eos );

    return in.good();
}

//----------------------------------------------------------------------------
/*! 
 *  Open source file and add lines to vector.
 */

bool DFileSystemTools::readFileIntoVector(std::vector<std::string>& lines, const std::string& source)
{
    lines.clear();
    ifstream in( source.c_str() );
    if ( !in )
    {
        return false;
    }
    
    return readStreamIntoVector( lines, in );
}


//----------------------------------------------------------------------------
/*! 
 *  Add lines from stream to vector.
 */

bool DFileSystemTools::readStreamIntoVector(std::vector<std::string>& lines, istream& in)
{
    string line;
    while ( std::getline(in, line) )
    {
        lines.push_back( line );
    }

    return true;
}


//----------------------------------------------------------------------------
/*! 
 *  Get free and total disk space in a folder.
 */

bool DFileSystemTools::freeDiskSpace(unsigned long long& freeBytes, unsigned long long& totalBytes, const std::string& folder)
{
#if defined ( D_OS_WIN32 )
    ULARGE_INTEGER f, t;

    BOOL ok = GetDiskFreeSpaceExW(
        boost::locale::conv::utf_to_utf<wchar_t>( folder ).c_str(),
        &f,
        &t,
        NULL );

    freeBytes = f.QuadPart;
    totalBytes = t.QuadPart;

    return ok != 0;
#else
    struct statvfs buf;

    if ( statvfs( folder.c_str(), &buf ) != 0 )
    {
        return false;
    }

    unsigned long long blksize, blocks, freeblks;
    blksize = buf.f_bsize;
    blocks = buf.f_blocks;
    freeblks = buf.f_bfree;
    
    totalBytes = blocks * blksize;
    freeBytes  = freeblks * blksize;
    
    return true;
#endif
}

//============================================================================
//  P R I V A T E   I N T E R F A C E
//============================================================================

/********************************** EOF *************************************/
