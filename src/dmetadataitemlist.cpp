/*****************************************************************************
**
**  Metadata item list class
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
#include    "dmetadataitemlist.h"

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
/*! \class DMetadataItemList dmetadataitemlist.h
 *  \ingroup PiqlIngest
 *  \brief Class for managing the list of metadata items.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::DMetadataItemList()
 *
 *  Constructor
 */

DMetadataItemList::DMetadataItemList()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::~DMetadataItemList()
 *
 *  Destructor
 */

DMetadataItemList::~DMetadataItemList()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::count()
 *
 *  Return number of items in list
 */

unsigned int DMetadataItemList::count() const
{
    return m_MetadataList.size();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::isValid()
 *
 *  Check if items in list are valid
 */

bool DMetadataItemList::isValid() const
{
    ERROR_F( "DMetadataItemList::isValid" );

    // Check that every individual item is valid
    for ( unsigned int i = 0; i < m_MetadataList.size(); i++ )
    {
        if ( !m_MetadataList[i].isValid() )
        {
            error << ERRerror << "Invalid item at index " << i << endl;
            return false;
        }
    }

    // Multiple keys of same name is not allowed
    for ( unsigned int i = 0; i < m_MetadataList.size(); i++ )
    {
        for ( unsigned int j = i + 1; j < m_MetadataList.size(); j++ )
        {
            if ( m_MetadataList[i].key() == m_MetadataList[j].key() )
            {
                error << ERRerror << "Duplicate key: " << m_MetadataList[i].key() << endl;
                return false;
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::addItem()
 *
 *  Add item to list
 */

void DMetadataItemList::addItem( const DMetadataItem& item )
{
    m_MetadataList.push_back( item );
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::getItem()
 *
 *  Get item from list
 */

bool DMetadataItemList::getItem( DMetadataItem& item, unsigned int index ) const
{
    ERROR_F( "DMetadataItemList::getItem" );

    if ( index >= count() )
    {
        error << ERRerror << "Index is out of range" << endl;
        return false;
    }

    item = m_MetadataList[index];
    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::getItem()
 *
 *  Get item from list
 */

bool DMetadataItemList::getItem( DMetadataItem& item, const std::string& key ) const
{
    ERROR_F( "DMetadataItemList::getItem" );

    for ( unsigned int i = 0; i < m_MetadataList.size(); i++ )
    {
        if ( m_MetadataList[i].key() == key )
        {
            item = m_MetadataList[i];
            return true;
        }
    }

    error << ERRerror << "No item found with key: " << key << endl;
    return false;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::clear()
 *
 *  Clear list
 */

void DMetadataItemList::clear()
{
    m_MetadataList.clear();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::write()
 *
 *  Write list
 */

bool DMetadataItemList::write( std::ostream& out, bool header ) const
{
    ERROR_F( "DMetadataItemList::write" );

    if ( header )
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << endl;
    }
    for ( unsigned int i = 0; i < m_MetadataList.size(); i++ )
    {
        if ( !m_MetadataList[i].write( out ) )
        {
            error << ERRerror << "Failed to write item" << endl;
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::Read()
*
*  Read list
*/

bool DMetadataItemList::Read( DMetadataItemList& itemList, QDomElement& element )
{
    ERROR_F( "DMetadataItemList::Read" );

    itemList.clear();

    QDomNode child = element.firstChild();
    while ( !child.isNull() )
    {
        QDomElement elem = child.toElement();

        DMetadataItem item;
        if ( !DMetadataItem::Read(item, elem) )
        {
            error << ERRerror << "Failed to read item" << endl;
            return false;
        }

        itemList.addItem(item);

        child = child.nextSibling();
    }

    return true;
}
