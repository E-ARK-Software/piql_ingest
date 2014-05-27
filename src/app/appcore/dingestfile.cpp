/*****************************************************************************
**
**  Ingest file class
**
**  Creation date:  2017-04-04
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
#include    "dingestfile.h"

//  BASE INCLUDES
//
#include    "dpath.h"

//  QT INCLUDES
//
#include    <QDomNode>

//  SYSTEM INCLUDES
//

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )


/****************************************************************************/
/*! \class DIngestFile dingestfile.h
 *  \ingroup PiqlIngest
 *  \brief Class for managing an ingest file.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::DIngestFile()
 *
 *  Constructor
 */

DIngestFile::DIngestFile()
{
}


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::DIngestFile()
 *
 *  Constructor
 */

DIngestFile::DIngestFile( const string& fileName ) :
    m_FileName( fileName ),
    m_Type( DIngestFile::TYPE_UNDEFINED ),
    m_Id( DIngestFile::FILE_ID_UNDEFINED ),
    m_ParentId( DIngestFile::FILE_ID_UNDEFINED )
{
}


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::DIngestFile()
*
*  Constructor
*/

DIngestFile::DIngestFile( const std::string& fileName, DIngestFile::Type type, int id, int parentId ) :
    m_FileName( fileName ),
    m_Type( type ),
    m_Id( id ),
    m_ParentId( parentId )
{
}


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::~DIngestFile()
 *
 *  Destructor
 */

DIngestFile::~DIngestFile()
{
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFile::isValid()
 *
 *  Return true if valid
 */

bool DIngestFile::isValid() const
{
    ERROR_F( "DIngestFile::isValid" );

    DPath file( m_FileName );
    if ( !DPath::isExistingPath( file ) )
    {
        error << ERRerror << "File does not exist on disk: " << m_FileName << endl;
        return false;
    }

    if ( !m_MetadataGroupList.isValid() )
    {
        error << ERRerror << "Metadata is invalid" << endl;
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFile::fileName()
 *
 *  Return filename
 */

string DIngestFile::fileName( bool fullPath ) const
{
    DPath file( m_FileName );
    if ( fullPath )
    {
        return file.path();
    }
    else
    {
        if ( m_Type == DIngestFile::TYPE_DIRECTORY )
        {
            vector< string > paths = file.pathList();
            if ( paths.size() > 1 )
            {
                return paths[paths.size() - 2];
            }
            return file.directory();
        }
        return file.filename();
    }
}


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::id()
*
*  Return id
*/

int DIngestFile::id() const
{
    return m_Id;
}


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::parentId()
*
*  Return parent id
*/

int DIngestFile::parentId() const
{
    return m_ParentId;
}


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::type()
*
*  Return type
*/

DIngestFile::Type DIngestFile::type() const
{
    return m_Type;
}


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::typeAsString()
*
*  Return type
*/

string DIngestFile::typeAsString() const
{
    if ( m_Type == DIngestFile::TYPE_FILE )
    {
        return "file";
    }
    else if ( m_Type == DIngestFile::TYPE_DIRECTORY )
    {
        return "directory";
    }

    return "unknown";
}


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::hasParent()
*
*  Return if file has a parent
*/

bool DIngestFile::hasParent() const
{
    return m_ParentId != FILE_ID_UNDEFINED && m_ParentId != FILE_ID_NONE;
}


//----------------------------------------------------------------------------
/*! \fn     DIngestFile::metadataGroupList()
 *
 *  Return list of metadata groups for file
 */

const DMetadataItemGroupList& DIngestFile::metadataGroupList() const
{
    return m_MetadataGroupList;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFile::setMetadataGroups()
 *
 *  Set metadata
 */

void DIngestFile::setMetadataGroups( const DMetadataItemGroupList& metadataGroupList )
{
    m_MetadataGroupList = metadataGroupList;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFile::setFileName()
*
*  Set file name
*/

void DIngestFile::setFileName( const string& fileName )
{
    m_FileName = fileName;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFile::setId()
*
*  Set id
*/

void DIngestFile::setId( int id )
{
    m_Id = id;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFile::setParentId()
*
*  Set parent id
*/

void DIngestFile::setParentId( int id )
{
    m_ParentId = id;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFile::setType()
*
*  Set type
*/

void DIngestFile::setType( DIngestFile::Type type )
{
    m_Type = type;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFile::write()
*
*  Write file
*/

bool DIngestFile::write( std::ostream& out, bool header ) const
{
    ERROR_F("DIngestFile::write");

    const string baseIndentation = header ? "" : "   ";

    if ( header )
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << endl;
    }
    out << baseIndentation << "<file>" << endl;
    out << baseIndentation << " <filename><![CDATA[" << m_FileName << "]]></filename>" << endl;
    out << baseIndentation << " <id>" << m_Id << "</id>" << endl;
    out << baseIndentation << " <parentId>" << m_ParentId << "</parentId>" << endl;
    out << baseIndentation << " <type>" << m_Type << "</type>" << endl;
    if ( !m_MetadataGroupList.write(out, false) )
    {
        error << ERRerror << "Failed to write metadata group list" << endl;
        return false;
    }
    out << baseIndentation << "</file>" << endl;

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DIngestFile::write()
*
*  Read file
*/

bool DIngestFile::Read( DIngestFile& item, QDomElement& element )
{
    ERROR_F("DIngestFile::Read");

    item = DIngestFile();

    QDomNode entries = element.firstChild();
    while ( !entries.isNull() )
    {
        QDomElement data = entries.toElement();
        QString tagName = data.tagName();

        if ( tagName == "filename" )
        {
            item.setFileName( data.text().toStdString() );
        }
        else if ( tagName == "id" )
        {
            item.setId( data.text().toInt() );
        }
        else if ( tagName == "parentId" )
        {
            item.setParentId( data.text().toInt() );
        }
        else if ( tagName == "type" )
        {
            item.setType( (DIngestFile::Type)data.text().toInt() );
        }
        else if ( tagName == "metadata" )
        {
            DMetadataItemGroupList metadataGroupList;
            if ( !DMetadataItemGroupList::Read(metadataGroupList, data) )
            {
                error << ERRerror << "Failed to read item groups" << endl;
                return false;
            }

            item.setMetadataGroups( metadataGroupList );
        }
        else if ( tagName == "" )
        {
            // Comments
        }
        else
        {
            error << ERRerror << "Unknown tag: " << tagName.toStdString() << endl;
            return false;
        }
        entries = entries.nextSibling();
    }

    return true;
}
