/*****************************************************************************
**
**  Metadata template item group list class
**
**  Creation date:  2017-12-01
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
#include    "dmetadatatemplateitemgrouplist.h"
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
/*! \class DMetadataTemplateItemGroupList dmetadatatemplateitemgrouplist.h
 *  \ingroup PiqlIngest
 *  \brief Class for holding metadata item group list.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroupList::DMetadataTemplateItemGroupList()
 *
 *  Constructor
 */

DMetadataTemplateItemGroupList::DMetadataTemplateItemGroupList()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroupList::~DMetadataTemplateItemGroupList()
 *
 *  Destructor
 */

DMetadataTemplateItemGroupList::~DMetadataTemplateItemGroupList()
{
    clear();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroupList::clear()
 *
 *  Clear all info
 */

void DMetadataTemplateItemGroupList::clear()
{
    m_GroupList.clear();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroupList::addGroup()
 *
 *  Add group to list
 */

bool DMetadataTemplateItemGroupList::addGroup( const DMetadataTemplateItemGroup& group )
{
    m_GroupList.push_back( group );

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroupList::Read( DMetadataTemplateItemGroupList& groupList, QDomElement& element )
 *
 *  Read list
 */

bool DMetadataTemplateItemGroupList::Read( DMetadataTemplateItemGroupList& groupList, QDomElement& element )
{
    ERROR_F( "DMetadataTemplateItemList::Read" );

    groupList.clear();
    QDomNodeList children = element.elementsByTagName( "group" );

    if ( !children.size() )
    {
        error << ERRinfo << "Empty metadata section" << endl;
        return true;
    }
    
    for ( int i = 0; i < children.size(); i++ )
    {
        QDomElement elem = children.at( i ).toElement();
        
        DMetadataTemplateItemGroup group;
        if ( !DMetadataTemplateItemGroup::Read(group, elem) )
        {
            error << ERRerror << "Failed to read item with index: " << i << endl;
            return false;
        }
        
        if ( !groupList.addGroup(group) )
        {
            error << ERRerror << "Failed to add item to list" << endl;
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroupList::groupCount()
 *
 *  Return item count
 */

unsigned int DMetadataTemplateItemGroupList::groupCount() const
{
    return m_GroupList.size();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroupList::getGroup()
 *
 *  Return item
 */

bool DMetadataTemplateItemGroupList::getGroup( DMetadataTemplateItemGroup& group, unsigned int index ) const
{
    ERROR_F( "DMetadataTemplateItemGroupList::getGroup" );

    if ( index >= groupCount() )
    {
        error << ERRinfo << "Index is out of range" << endl;
        return false;
    }

    group = m_GroupList[index];

    return true;
}
