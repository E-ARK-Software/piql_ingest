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
#include    "dmetadataitemgrouplist.h"

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
/*! \class DMetadataItemGroupList dmetadataitemgrouplist.h
 *  \ingroup PiqlIngest
 *  \brief Class for managing the list of metadata item groups.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroupList::DMetadataItemGroupList()
 *
 *  Constructor
 */

DMetadataItemGroupList::DMetadataItemGroupList()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroupList::~DMetadataItemGroupList()
 *
 *  Destructor
 */

DMetadataItemGroupList::~DMetadataItemGroupList()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroupList::groupCount()
 *
 *  Return number of groups in list
 */

unsigned int DMetadataItemGroupList::groupCount() const
{
    return m_MetadataGroupList.size();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroupList::isValid()
 *
 *  Check if items in list are valid
 */

bool DMetadataItemGroupList::isValid() const
{
    ERROR_F( "DMetadataItemList::isValid" );

    // Check that every individual item is valid
    for ( unsigned int i = 0; i < m_MetadataGroupList.size(); i++ )
    {
        if ( !m_MetadataGroupList[i].isValid() )
        {
            error << ERRerror << "Invalid item at index " << i << endl;
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::addGroup()
 *
 *  Add item to list
 */

void DMetadataItemGroupList::addGroup( const DMetadataItemGroup& group )
{
    m_MetadataGroupList.push_back( group );
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::getItem()
 *
 *  Get item from list
 */

bool DMetadataItemGroupList::getGroup( DMetadataItemGroup& group, unsigned int index ) const
{
    ERROR_F( "DMetadataItemGroupList::getGroup" );

    if ( index >= groupCount() )
    {
        error << ERRerror << "Index is out of range" << endl;
        return false;
    }

    group = m_MetadataGroupList[index];
    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroupList::clear()
 *
 *  Clear list
 */

void DMetadataItemGroupList::clear()
{
    m_MetadataGroupList.clear();
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemList::write()
 *
 *  Write list
 */

bool DMetadataItemGroupList::write( std::ostream& out, bool header ) const
{
    ERROR_F( "DMetadataItemGroupList::write" );

    if ( header )
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << endl;
    }
    out << "<metadata>";
    for ( unsigned int i = 0; i < m_MetadataGroupList.size(); i++ )
    {
        if ( !m_MetadataGroupList[i].write(out) )
        {
            error << ERRerror << "Failed to write item" << endl;
            return false;
        }
    }
    out << "</metadata>";

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItemGroupList::Read()
*
*  Read list
*/

bool DMetadataItemGroupList::Read( DMetadataItemGroupList& groupList, QDomElement& element )
{
    ERROR_F( "DMetadataItemGroupList::Read" );

    groupList.clear();

    QDomNodeList children = element.elementsByTagName("group");
    for ( int i = 0; i < children.size(); i++ )
    {
        QDomElement elem = children.at(i).toElement();

        DMetadataItemGroup item;
        if ( !DMetadataItemGroup::Read(item, elem) )
        {
            error << ERRerror << "Failed to read item" << endl;
            return false;
        }

        groupList.addGroup( item );
    }

    return true;
}
