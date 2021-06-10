/*****************************************************************************
**
**  Metadata template item group class
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
#include    "dmetadatatemplateitemgroup.h"
#include    "derror.h"
#include    "dqterror.h"

//  QT INCLUDES
//
#include    <QTextStream>
#include    <QDomDocument>

//  SYSTEM INCLUDES
//

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )


/****************************************************************************/
/*! \class DMetadataTemplateItemGroup dmetadatatemplateitemgroup.h
 *  \ingroup PiqlIngest
 *  \brief Class for holding metadata item group.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::DMetadataTemplateItemGroup()
 *
 *  Constructor
 */

DMetadataTemplateItemGroup::DMetadataTemplateItemGroup()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::DMetadataTemplateItemGroup()
 *
 *  Constructor
 */

DMetadataTemplateItemGroup::DMetadataTemplateItemGroup(
    const std::string& name,
    const std::string& description ) :
    m_Name( name ),
    m_Description( description )
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::~DMetadataTemplateItemGroup()
 *
 *  Destructor
 */

DMetadataTemplateItemGroup::~DMetadataTemplateItemGroup()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::name()
 *
 *  Return name
 */

string DMetadataTemplateItemGroup::name() const
{
    return m_Name;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::description()
 *
 *  Return description
 */

string DMetadataTemplateItemGroup::description() const
{
    return m_Description;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::setName()
 *
 *  Set name
 */

void DMetadataTemplateItemGroup::setName( const string& name )
{
    m_Name = name;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::setDescription()
 *
 *  Set description
 */

void DMetadataTemplateItemGroup::setDescription( const string& description )
{
    m_Description = description;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::setItems()
 *
 *  Set metadata item list
 */

void DMetadataTemplateItemGroup::setItems( const DMetadataTemplateItemList& items )
{
    m_ItemList = items;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::itemCount()
*
*  Return metadata item count
*/

unsigned int DMetadataTemplateItemGroup::itemCount() const
{
    return m_ItemList.count();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::getItem()
*
*  Get metadata template item
*/

bool DMetadataTemplateItemGroup::getItem( DMetadataTemplateItem& item, unsigned int index ) const
{
    ERROR_F( "DMetadataTemplateItemGroup::getItem" );

    if ( !m_ItemList.getItem(item, index) )
    {
        error << ERRerror << "Failed to get item from list" << endl;
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItemGroup::Read()
 *
 *  Read group
 */

bool DMetadataTemplateItemGroup::Read( DMetadataTemplateItemGroup& group, QDomElement& element )
{
    ERROR_F( "DMetadataTemplateItemGroup::Read" );
    
    group = DMetadataTemplateItemGroup();

    // Name
    QString name = element.attribute( "name" );
    if ( name.isEmpty() )
    {
        error << ERRerror << "Missing attribute 'name'" << endl;
        return false;
    }
    group.setName( name.toStdString() );

    // Description
    QString description = element.attribute( "description" );
    if ( description.isEmpty() )
    {
        error << ERRerror << "Missing attribute 'description'" << endl;
        return false;
    }
    group.setDescription( description.toStdString() );

    DMetadataTemplateItemList itemList;
    if ( !DMetadataTemplateItemList::Read(itemList, element) )
    {
        error << ERRerror << "Failed to read metadata list" << endl;
        return false;
    }
    group.setItems( itemList );

    return true;
}
