/*****************************************************************************
**
**  Metadata template item list class
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
#include    "dmetadatatemplateitemlist.h"
#include    "derror.h"

//  QT INCLUDES
//
#include    <QDomNodeList>

//  SYSTEM INCLUDES
//

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )


/****************************************************************************/
/*! \class DMetadataTemplateItemList dmetadatatemplateitemlist.h
 *  \ingroup PiqlIngest
 *  \brief Class for holding metadata item info list.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemList::DMetadataTemplateItemList()
 *
 *  Constructor
 */

DMetadataTemplateItemList::DMetadataTemplateItemList()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemList::~DMetadataTemplateItemList()
 *
 *  Destructor
 */

DMetadataTemplateItemList::~DMetadataTemplateItemList()
{
    clear();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemList::clear()
 *
 *  Clear all info
 */

void DMetadataTemplateItemList::clear()
{
    m_ItemList.clear();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemList::addItem()
 *
 *  Add item to list
 */

bool DMetadataTemplateItemList::addItem( const DMetadataTemplateItem& item )
{
    m_ItemList.push_back( item );

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemList::Read( DMetadataTemplateItemList& itemList, QDomElement& element )
 *
 *  Read list
 */

bool DMetadataTemplateItemList::Read( DMetadataTemplateItemList& itemList, QDomElement& element )
{
    ERROR_F( "DMetadataTemplateItemList::Read" );

    itemList.clear();
    QDomNodeList children = element.elementsByTagName( "item" );

    if ( !children.size() )
    {
        error << ERRinfo << "Empty metadata section" << endl;
        return true;
    }
    
    for ( int i = 0; i < children.size(); i++ )
    {
        QDomElement elem = children.at( i ).toElement();
        
        DMetadataTemplateItem item;
        if ( !DMetadataTemplateItem::Read( item, elem ) )
        {
            error << ERRerror << "Failed to read item with index: " << i << endl;
            return false;
        }
        
        if ( !itemList.addItem( item ) )
        {
            error << ERRerror << "Failed to add item to list" << endl;
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemList::count()
 *
 *  Return item count
 */

unsigned int DMetadataTemplateItemList::count() const
{
    return m_ItemList.size();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemList::getItem()
 *
 *  Return item
 */

bool DMetadataTemplateItemList::getItem( DMetadataTemplateItem& item, unsigned int index ) const
{
    ERROR_F( "DMetadataTemplateItemList::getItem" );

    if ( index >= count() )
    {
        error << ERRinfo << "Index is out of range" << endl;
        return false;
    }

    item = m_ItemList[index];

    return true;
}
