/*****************************************************************************
**
**  Metadata item class
**
**  Creation date:  2017-04-05
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
#include    "dmetadataitem.h"

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
/*! \class DMetadataItem dmetadataitem.h
 *  \ingroup PiqlIngest
 *  \brief Class for managing a metadata item.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================


//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::DMetadataItem()
 *
 *  Constructor
 */

DMetadataItem::DMetadataItem()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::DMetadataItem()
 *
 *  Constructor
 */

DMetadataItem::DMetadataItem( const string& key, const string& value ) :
    m_Key( key ),
    m_Value( value )
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::~DMetadataItem()
 *
 *  Destructor
 */

DMetadataItem::~DMetadataItem()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::isValid()
 *
 *  Return true if valid
 */

bool DMetadataItem::isValid() const
{
    ERROR_F( "DMetadataItem::isValid" );
    if ( m_Key.length() == 0 )
    {
        error << ERRerror << "Key is empty" << endl;
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::key()
 *
 *  Return key
 */

string DMetadataItem::key() const
{
    return m_Key;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::value()
 *
 *  Return value
 */

string DMetadataItem::value() const
{
    return m_Value;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::setKey()
*
*  Set key
*/

void DMetadataItem::setKey( const string& key )
{
    m_Key = key;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::setValue()
*
*  Set value
*/

void DMetadataItem::setValue( const string& value )
{
    m_Value = value;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::write()
 *
 *  Write item
 */

bool DMetadataItem::write( std::ostream& out ) const
{
    out << "<" << m_Key << "><![CDATA[" << m_Value << "]]></" << m_Key << ">";

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataItem::Read()
*
*  Read item
*/

bool DMetadataItem::Read( DMetadataItem& item, QDomElement& element )
{
    ERROR_F( "DMetadataItem::Read" );

    item.setKey(element.tagName().toStdString());
    item.setValue(element.text().toStdString());

    return true;
}
