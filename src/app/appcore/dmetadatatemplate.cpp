/*****************************************************************************
**
**  Configuration class
**
**  Creation date:  2017-04-03
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
#include    "dmetadatatemplate.h"
#include    "derror.h"

//  SYSTEM INCLUDES
//

//  QT INCLUDES
//
#include    <QFile>
#include    <QDomDocument>

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )


/****************************************************************************/
/*! \class DMetadataTemplate dmetadatatemplate.h
 *  \ingroup PiqlIngest
 *  \brief Class for holding metadata.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplate::DMetadataTemplate()
 *
 *  Constructor
 */

DMetadataTemplate::DMetadataTemplate()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplate::~DMetadataTemplate()
 *
 *  Destructor
 */

DMetadataTemplate::~DMetadataTemplate()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplate::read()
 *
 *  Read definition
 */

bool DMetadataTemplate::read( std::istream& definition )
{
    std::istreambuf_iterator<char> it;
    string data( std::istreambuf_iterator<char>(definition), it );

    QDomDocument dom;
    dom.setContent( QString::fromStdString( data ) );

    return read( dom );
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplate::read()
 *
 *  Read definition
 */

bool DMetadataTemplate::read( const std::string& fileName )
{
    ERROR_F( "DMetadataTemplate::read" );

    // Clear object;
    *this = DMetadataTemplate();

    QFile file( QString::fromStdString( fileName ) );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        error << ERRerror << "Failed to open: " << fileName << endl;
        return false;
    }

    QDomDocument doc("metadataform");
    QString errorMessage;
    int errorLine;
    int errorColumn;
    if ( !doc.setContent( &file, &errorMessage, &errorLine, &errorColumn ) )
    {
        error << ERRerror 
              << "Failed to set document content. "
              << "File=" << fileName 
              << " Error='" << errorMessage.toStdString() 
              << "' at line=" << errorLine 
              << " column=" << errorColumn 
              << endl;

        return false;
    }
    file.close();

    return read( doc );
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplate::read()
 *
 *  Read definition
 */

bool DMetadataTemplate::read( QDomDocument& doc )
{
    ERROR_F( "DMetadataTemplate::read" );

    m_CollectionMetadata.clear();
    m_FileMetadataGroups.clear();

    // Get the root element
    QDomElement docElem = doc.documentElement();

    // Check root tag name
    QString rootTag = docElem.tagName();
    if ( rootTag != "metadata-form" )
    {
        error << ERRerror << "metadata-form is not root element but: " << rootTag.toStdString() << endl;
        return false;
    }

    // Read collection-metadata
    QDomNodeList collectionMetadataList = docElem.elementsByTagName("collection-metadata");
    if ( collectionMetadataList.size() == 1 )
    {
        QDomElement collectionMetadataElement = collectionMetadataList.at(0).toElement();
        if ( !DMetadataTemplateItemList::Read( m_CollectionMetadata, collectionMetadataElement ) )
        {
            error << ERRerror << "failed to read collection-metadata section" << endl;
            return false;
        }
    }
    else if ( collectionMetadataList.size() > 1)
    {
        error << ERRerror << "more than one collection-metadata tag found" << endl;
        return false;
    }
    else
    {
        error << ERRinfo << "No collection-metadata tag found" << endl;
    }

    // Read file-metadata
    QDomNodeList fileMetadataGroupList = docElem.elementsByTagName("file-metadata");
    if ( fileMetadataGroupList.size() == 1 )
    {
        QDomElement fileMetadataGroupElement = fileMetadataGroupList.at(0).toElement();
        if ( !DMetadataTemplateItemGroupList::Read( m_FileMetadataGroups, fileMetadataGroupElement ) )
        {
            error << ERRerror << "failed to read file-metadata section" << endl;
            return false;
        }
    }
    else if ( fileMetadataGroupList.size() > 1 )
    {
        error << ERRerror << "more than one file-metadata tag found" << endl;
        return false;
    }
    else
    {
        error << ERRinfo << "No file-metadata tag found" << endl;
    }
    
    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplate::collectionMetadata()
 *
 *  Return collection metadata
 */

const DMetadataTemplateItemList& DMetadataTemplate::collectionMetadata() const
{
    return m_CollectionMetadata;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplate::fileMetadataGroups()
 *
 *  Return file metadata groups
 */

const DMetadataTemplateItemGroupList& DMetadataTemplate::fileMetadataGroups() const
{
    return m_FileMetadataGroups;
}
