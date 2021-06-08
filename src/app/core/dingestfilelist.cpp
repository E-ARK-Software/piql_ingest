/*****************************************************************************
**
**  Configuration class
**
**  Creation date:  2017-03-30
**  Created by:     Matti Wennberg
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the DLFR software util modules
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "dingestfilelist.h"

//  BASE INCLUDES
//
#include    "dpath.h"

//  QT INCLUDES
//
#include    <QDomNodeList>

//  SYSTEM INCLUDES
//

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )


/****************************************************************************/
/*! \class DIngestFileList dingestfilelist.h
 *  \ingroup PiqlIngest
 *  \brief Class for managing the list of ingest files.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::DIngestFileList()
 *
 *  Constructor
 */

DIngestFileList::DIngestFileList()
{
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::~DIngestFileList()
 *
 *  Destructor
 */

DIngestFileList::~DIngestFileList()
{
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::count()
 *
 *  Return number of files in list
 */

unsigned int DIngestFileList::count() const
{
    return m_FileList.size();
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::addFile()
 *
 *  Add file to list
 */

unsigned int DIngestFileList::addFile( const DIngestFile& file )
{
    m_FileList.push_back( file );
    return m_FileList.size() - 1;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::addFileFromDisk()
*
*  Add file to list
*/

bool DIngestFileList::addFileFromDisk( vector<int>& ids, const string& fileName, int parentId /*= DIngestFile::FILE_ID_NONE*/ )
{
    ERROR_F("DIngestFileList::addFileFromDisk");

    DPath file( fileName );

    error << ERRinfo << "File info: Path='" << file.path() << "' Directory='" << file.directory() << "' Filename='" << file.filename() << "'" << endl;

    // Is directory?
    if ( file.filename().empty() )
    {
        error << ERRinfo << "Adding as directory: " << file.path() << endl;

        // Add index for directory
        int dirId = generateId();
        addFile( DIngestFile(file.directory(), DIngestFile::TYPE_DIRECTORY, dirId, parentId) );
        ids.push_back( dirId );

        // Search directory for files
        vector<string> files;
        if ( !DPath::scanDir(file, "*", files, true, false, true, true, true) )
        {
            return false;
        }

        // Add files
        for ( unsigned int i = 0; i < files.size(); i++ )
        {
            // Is directory?
            DPath subFile( files[i] );
            if ( subFile.filename().empty() )
            {
                if ( !addFileFromDisk(ids, files[i], dirId) )
                {
                    return false;
                }
            }
            else
            {
                error << ERRinfo << "Adding as file: " << subFile.path() << endl;
                int id = generateId();
                addFile( DIngestFile(subFile.path(), DIngestFile::TYPE_FILE, id, dirId) );
                ids.push_back( id );
            }
        }

        return true;
    }

    // Is file
    error << ERRinfo << "Adding as file: " << file.path() << endl;
    int id = generateId();
    addFile( DIngestFile(file.path(), DIngestFile::TYPE_FILE, id, parentId) );
    ids.push_back( id );
    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::getFileByIndex()
 *
 *  Get file from list
 */

bool DIngestFileList::getFileByIndex(DIngestFile& ingestFile, unsigned int index) const
{
    ERROR_F( "DIngestFileList::getFileByIndex" );

    if ( index >= count() )
    {
        error << ERRerror << "Index is out of range" << endl;
        return false;
    }

    ingestFile = m_FileList[index];
    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::getFileById()
*
*  Get file from list
*/

bool DIngestFileList::getFileById( DIngestFile& ingestFile, int id ) const
{
    ERROR_F("DIngestFileList::getFileById");

    return getFileByIndex( ingestFile, idToIndex(id) );
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::idExists()
*
*  Check if a file with given id exists
*/

bool DIngestFileList::idExists( int id ) const
{
    ERROR_F("DIngestFileList::idExists");

    for ( unsigned int i = 0; i < m_FileList.size(); i++ )
    {
        if ( m_FileList[i].id() == id )
        {
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::getRelativePath()
*
*  Get relative path of file
*/

bool DIngestFileList::getRelativePath(std::string& path, const DIngestFile& ingestFile) const
{
    path = ingestFile.fileName(false) + path;

    if ( ingestFile.hasParent() )
    {
        for ( unsigned int i = 0; i < m_FileList.size(); i++ )
        {
            if ( m_FileList[i].id() == ingestFile.parentId() )
            {
                path = DPath::dirSeparator() + path;
                return getRelativePath( path, m_FileList[i] );
            }
        }
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::setFileByIndex()
 *
 *  Set file in list
 */

bool DIngestFileList::setFileByIndex( const DIngestFile& ingestFile, unsigned int index )
{
    ERROR_F( "DIngestFileList::setFileByIndex" );

    if ( index >= count() )
    {
        error << ERRerror << "Index is out of range" << endl;
        return false;
    }

    m_FileList[index] = ingestFile;
    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::setFileById()
*
*  Set file in list
*/

bool DIngestFileList::setFileById( const DIngestFile& ingestFile, int id )
{
    ERROR_F( "DIngestFileList::setFileById" );

    return setFileByIndex( ingestFile, idToIndex(id) );
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::setFiles()
*
*  Set files in list
*/

bool DIngestFileList::setFilesByIndex(const DIngestFileList& ingestFiles, std::vector< unsigned int > indexes)
{
    ERROR_F("DIngestFileList::setFiles");

    // Matching indexes and files?
    if (ingestFiles.count() != indexes.size())
    {
        return false;
    }

    // Out of range indexes?
    for (unsigned int i = 0; i < indexes.size(); i++)
    {
        if (indexes[i] >= m_FileList.size())
        {
            return false;
        }
    }

    // Backup old list
    vector<DIngestFile> originalFileList = m_FileList;

    // Replace files
    for (unsigned int i = 0; i < indexes.size(); i++)
    {
        DIngestFile file;
        if ( !ingestFiles.getFileByIndex(file, i) )
        {
            error << ERRerror << "Failed to get file from list" << endl;
            m_FileList = originalFileList;
            return false;
        }
        m_FileList[indexes[i]] = file;
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::setFilesById()
*
*  Set files in list
*/

bool DIngestFileList::setFilesById( const DIngestFileList& ingestFiles, std::vector< int > ids )
{
    ERROR_F( "DIngestFileList::setFilesById" );

    vector<unsigned int> indexes;
    for ( unsigned int i = 0; i < ids.size(); i++ )
    {
        indexes.push_back( idToIndex(ids[i]) );
    }

    return setFilesByIndex( ingestFiles, indexes );
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::isValid()
*
*  Are all files in list valid?
*/

bool DIngestFileList::isValid() const
{
    ERROR_F("DIngestFileList::isValid");

    for ( unsigned int i = 0; i < m_FileList.size(); i++ )
    {
        if ( !m_FileList[i].isValid() )
        {
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::removeFileByIndex()
 *
 *  Remove file from list
 */

bool DIngestFileList::removeFileByIndex( unsigned int index )
{
    ERROR_F( "DIngestFileList::removeFileByIndex" );

    if ( index >= count() )
    {
        error << ERRerror << "Index is out of range" << endl;
        return false;
    }

    return removeFileById( m_FileList[index].id() );
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::removeFileById()
*
*  Remove file from list
*/

bool DIngestFileList::removeFileById( int id )
{
    ERROR_F("DIngestFileList::removeFileById");

    for ( int i = 0; (unsigned int)i < m_FileList.size(); i++ )
    {
        if ( m_FileList[i].parentId() == id )
        {
            if ( !removeFileById(m_FileList[i].id()) )
            {
                return false;
            }
            i = -1;
            continue;
        }
    }

    for ( unsigned int i = 0; i < m_FileList.size(); i++ )
    {
        if ( m_FileList[i].id() == id )
        {
            m_FileList.erase( m_FileList.begin() + i );
            return true;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::clear()
*
*  Clear list
*/

void DIngestFileList::clear()
{
    m_FileList.clear();
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::write()
*
*  Write list
*/

bool DIngestFileList::write( std::ostream& out, bool header ) const
{
    ERROR_F( "DIngestFileList::write" );

    if (header)
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << endl;
    }
    out << "<files>";
    for ( unsigned int i = 0; i < m_FileList.size(); i++ )
    {
        if ( !m_FileList[i].write(out, false) )
        {
            error << ERRerror << "Failed to write file" << endl;
            return false;
        }
    }
    out << "</files>";

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::generateId()
*
*  Generate a unique file id
*/

int DIngestFileList::generateId() const
{
    for ( int i = 0;; i++ )
    {
        bool isDuplicate = false;
        for ( unsigned int j = 0; j < m_FileList.size(); j++ )
        {
            if ( m_FileList[j].id() == i )
            {
                isDuplicate = true;
                break;
            }
        }

        if ( !isDuplicate )
        {
            return i;
        }
    }
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::idToIndex()
*
*  Get index from id
*/

unsigned int DIngestFileList::idToIndex( int id ) const
{
    ERROR_F( "DIngestFileList::idToIndex" );

    for ( unsigned int i = 0; i < m_FileList.size(); i++ )
    {
        if ( m_FileList[i].id() == id )
        {
            return i;
        }
    }

    unsigned int max = -1;
    return max;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFileList::Read()
*
*  Read list
*/

bool DIngestFileList::Read( DIngestFileList& itemList, QDomElement& element )
{
    ERROR_F( "DIngestFileList::Read" );

    // Clear info
    itemList.clear();

    // Read files
    QDomNodeList children = element.elementsByTagName("file");
    for ( int i = 0; i < children.size(); i++ )
    {
        QDomElement elem = children.at(i).toElement();

        DIngestFile item;
        if ( !DIngestFile::Read(item, elem) )
        {
            error << ERRerror << "Failed to read item with index: " << i << endl;
            return false;
        }

        itemList.addFile( item );
    }

    return true;
}
