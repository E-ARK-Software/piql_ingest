/*****************************************************************************
**
**  Metadata item class
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
#include    "dmetadataitemgroup.h"

//  BASE INCLUDES
//
#include    "dqterror.h"

//  SYSTEM INCLUDES
//

//  QT INCLUDES
//
#include    <QDomNode>

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )


/****************************************************************************/
/*! \class DMetadataItem dmetadataitemgroup.h
 *  \ingroup PiqlIngest
 *  \brief Class for managing a metadata item group.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================


//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::DMetadataItemGroup()
 *
 *  Constructor
 */

DMetadataItemGroup::DMetadataItemGroup()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::DMetadataItemGroup()
 *
 *  Constructor
 */

DMetadataItemGroup::DMetadataItemGroup( const string& name ) :
    m_Name( name )
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::~DMetadataItemGroup()
 *
 *  Destructor
 */

DMetadataItemGroup::~DMetadataItemGroup()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::isValid()
 *
 *  Return true if valid
 */

bool DMetadataItemGroup::isValid() const
{
    ERROR_F( "DMetadataItemGroup::isValid" );

    if ( m_Name.length() == 0 )
    {
        error << ERRerror << "Name is empty" << endl;
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::name()
 *
 *  Return name
 */

string DMetadataItemGroup::name() const
{
    return m_Name;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::count()
*
*  Return metadata item count
*/

unsigned int DMetadataItemGroup::itemCount() const
{
    return m_MetadataItems.count();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::setName()
*
*  Set name
*/

void DMetadataItemGroup::setName( const string& name )
{
    m_Name = name;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::addItem()
*
*  Set name
*/

void DMetadataItemGroup::addItem( const DMetadataItem& item )
{
    m_MetadataItems.addItem( item );
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::getItem()
*
*  Get metadata item
*/

bool DMetadataItemGroup::getItem( DMetadataItem& item, unsigned int index ) const
{
    ERROR_F( "DMetadataItemGroup::getItem" );

    return m_MetadataItems.getItem( item, index );
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::getItem()
*
*  Get metadata item
*/

bool DMetadataItemGroup::getItem( DMetadataItem& item, const string& key ) const
{
    ERROR_F( "DMetadataItemGroup::getItem" );

    return m_MetadataItems.getItem( item, key );
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::clear()
*
*  Clear data
*/

void DMetadataItemGroup::clear()
{
    ERROR_F("DMetadataItemGroup::clear");

    m_Name = string();
    m_MetadataItems.clear();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroup::write()
 *
 *  Write item
 */

bool DMetadataItemGroup::write( std::ostream& out ) const
{
    out << "<group name=\"" << m_Name << "\">";
    if ( !m_MetadataItems.write(out, false) )
    {
        return false;
    }
    out << "</group>";

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::Read()
*
*  Read item
*/

bool DMetadataItemGroup::Read(DMetadataItemGroup& group, QDomElement& element)
{
    ERROR_F( "DMetadataItem::Read" );

    group.clear();
    group.setName( element.attribute("name").toStdString() );
    if ( group.name().empty() )
    {
        error << ERRerror << "Group name is missing" << endl;
        return false;
    }

    QDomNode child = element.firstChild();
    while ( !child.isNull() )
    {
        QDomElement elem = child.toElement();

        DMetadataItem item;
        if ( !DMetadataItem::Read(item, elem) )
        {
            error << ERRerror << "Failed to read metadata item" << endl;
            return false;
        }

        group.addItem( item );

        child = child.nextSibling();
    }

    return true;
}
