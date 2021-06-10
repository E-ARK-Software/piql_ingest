/*****************************************************************************
**
**  Configuration class
**
**  Creation date:  2017-03-31
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
#include    "dmetadatatemplateitem.h"
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
/*! \class DMetadataTemplateItem dmetadatatemplateitem.h
 *  \ingroup PiqlIngest
 *  \brief Class for holding metadata item info.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::DMetadataTemplateItem()
 *
 *  Constructor
 */

DMetadataTemplateItem::DMetadataTemplateItem() :
    m_Type( METADATA_TEMPLATE_ITEM_TYPE_UNKNOWN )
{
    m_ReadOnly = false;
    m_Visible = true;
    m_Width = 200;
    m_FontSize = -1;
    m_LabelFontSize = -1;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::DMetadataTemplateItem()
 *
 *  Constructor
 */

DMetadataTemplateItem::DMetadataTemplateItem(
    const std::string& name,
    const std::string& description,
    const std::string& defaultValue,
    const std::string& data,
    unsigned int type,
    bool readOnly,
    bool visible,
    int fontSize,
    int labelFontSize,
    unsigned int width ) :
    m_Name( name ),
    m_Description( description ),
    m_DefaultValue ( defaultValue ),
    m_Data( data ),
    m_Type( type ),
    m_ReadOnly( readOnly ),
    m_Visible( visible ),
    m_Width( width ),
    m_FontSize( fontSize ),
    m_LabelFontSize( labelFontSize )
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::~DMetadataTemplateItem()
 *
 *  Destructor
 */

DMetadataTemplateItem::~DMetadataTemplateItem()
{
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::name()
 *
 *  Return name
 */

string DMetadataTemplateItem::name() const
{
    return m_Name;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::description()
 *
 *  Return description
 */

string DMetadataTemplateItem::description() const
{
    return m_Description;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::defaultValue()
 *
 *  Return defaultValue
 */

string DMetadataTemplateItem::defaultValue() const
{
    return m_DefaultValue;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::data()
 *
 *  Return data
 */

string DMetadataTemplateItem::data() const
{
    return m_Data;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::readOnly()
*
*  Return if item is read-only
*/

bool DMetadataTemplateItem::readOnly() const
{
    return m_ReadOnly;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::visible()
*
*  Return if item is visible
*/

bool DMetadataTemplateItem::visible() const
{
    return m_Visible;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::width()
*
*  Return item width in pixels
*/

unsigned int DMetadataTemplateItem::width() const
{
    return m_Width;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::fontSize()
*
*  Return font size in pixels
*/

int DMetadataTemplateItem::fontSize() const
{
    return m_FontSize;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::labelFontSize()
*
*  Return label font size in pixels
*/

int DMetadataTemplateItem::labelFontSize() const
{
    return m_LabelFontSize;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::type()
 *
 *  Return type
 */

unsigned int DMetadataTemplateItem::type() const
{
    return m_Type;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::getOptions()
*
*  Return options
*/

bool DMetadataTemplateItem::getOptions( std::vector<std::string>& names, std::vector<std::string>& values ) const
{
    ERROR_F( "DMetadataTemplateItem::getOptions" );

    names.clear();
    values.clear();

    QDomDocument dom;
    dom.setContent( QString::fromStdString(m_Data) );
    
    // Get the root element
    QDomElement docElem = dom.documentElement();

    // Check root tag name
    QString rootTag = docElem.tagName();
    if ( rootTag != "data" )
    {
        error << ERRerror << "data is not root element but: " << rootTag.toStdString() << endl;
        return false;
    }

    // Read options
    QDomNodeList optionsList = docElem.elementsByTagName("options");
    if ( optionsList.size() == 0 )
    {
        error << ERRinfo << "No options were found" << endl;
        return true;
    }
    else if ( optionsList.size() == 1 )
    {
        QDomElement optionsElement = optionsList.at(0).toElement();

        QDomNodeList children = optionsElement.elementsByTagName("option");

        for (int i = 0; i < children.size(); i++)
        {
            QDomElement elem = children.at(i).toElement();

            QDomNode entries = elem.firstChild();
            bool gotName = false;
            bool gotValue = false;
            string optionName;
            string optionValue;
            while (!entries.isNull())
            {
                QDomElement data = entries.toElement();
                QString tagName = data.tagName();

                if (tagName == "name")
                {
                    optionName = data.text().toStdString();
                    gotName = true;
                }
                else if (tagName == "value")
                {
                    optionValue = data.text().toStdString();
                    gotValue = true;
                }
                else if (tagName == "")
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

            if ( !gotName || !gotValue )
            {
                error << ERRerror << "Failed to find name and value for option" << endl;
                return false;
            }

            names.push_back( optionName );
            values.push_back( optionValue );
        }
    }
    else if ( optionsList.size() > 1 )
    {
        error << ERRerror << "more than one options tag found" << endl;
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::getSetting()
*
*  Return setting for key
*/

bool DMetadataTemplateItem::getSetting( std::string& value, const std::string& key ) const
{
    ERROR_F( "DMetadataTemplateItem::getOptions" );

    QDomDocument dom;
    dom.setContent( QString::fromStdString(m_Data) );
    
    // Get the root element
    QDomElement docElem = dom.documentElement();

    // Check root tag name
    QString rootTag = docElem.tagName();
    if ( rootTag != "data" )
    {
        error << ERRerror << "data is not root element but: " << rootTag.toStdString() << endl;
        return false;
    }

    // Read options
    QDomNodeList settingsList = docElem.elementsByTagName( QString::fromStdString(key) );
    if ( settingsList.size() == 0 )
    {
        error << ERRdebug << "Setting was not found:" << key << endl;
        return false;
    }
    else
    {
        error << ERRdebug << "Setting was found: " << key << endl;
        QDomElement settingsElement = settingsList.at(0).toElement();
        value = settingsElement.text().toStdString();
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::getSetting()
*
*  Return setting for key
*/

bool DMetadataTemplateItem::getSetting( bool& value, const std::string& key ) const
{
    value = false;

    std::string valueStr;
    if ( !getSetting(valueStr, key) )
    {
        return false;
    }

    if ( valueStr == "true" || valueStr == "TRUE" || valueStr == "1" )
    {
        value = true;
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::validate()
*
*  Validate metadata value based on type
*/

bool DMetadataTemplateItem::validate( const std::string& /*value*/ ) const
{
    if ( m_Type == METADATA_TEMPLATE_ITEM_TYPE_STRING )
    {
        
    }
    else if ( m_Type == METADATA_TEMPLATE_ITEM_TYPE_DROPDOWN )
    {
        
    }
    else if ( m_Type == METADATA_TEMPLATE_ITEM_TYPE_DATETIME )
    {
 
    }

    return true;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setName()
 *
 *  Set name
 */

void DMetadataTemplateItem::setName( const string& name )
{
    m_Name = name;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setDescription()
 *
 *  Set description
 */

void DMetadataTemplateItem::setDescription( const string& description )
{
    m_Description = description;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setDefaultValue()
 *
 *  Set defaultValue
 */

void DMetadataTemplateItem::setDefaultValue( const string& defaultValue )
{
    m_DefaultValue = defaultValue;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setData()
 *
 *  Set data
 */

void DMetadataTemplateItem::setData( const string& data )
{
    m_Data = data;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setType()
 *
 *  Set type
 */

void DMetadataTemplateItem::setType( unsigned int type )
{
    m_Type = type;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setReadOnly()
*
*  Set read-only status
*/

void DMetadataTemplateItem::setReadOnly( bool readOnly )
{
    m_ReadOnly = readOnly;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setVisible()
*
*  Set visible
*/

void DMetadataTemplateItem::setVisible(bool visible)
{
    m_Visible = visible;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setWidth()
*
*  Set item width in pixels
*/

void DMetadataTemplateItem::setWidth( unsigned int width )
{
    m_Width = width;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setFontSize()
*
*  Set font size in pixels
*/

void DMetadataTemplateItem::setFontSize( int fontSize )
{
    m_FontSize = fontSize;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::setFontSize()
*
*  Set label font size in pixels
*/

void DMetadataTemplateItem::setLabelFontSize(int fontSize)
{
    m_LabelFontSize = fontSize;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::GetType()
 *
 *  Return type
 */

unsigned int DMetadataTemplateItem::GetType( const std::string& type )
{
    unsigned int ret = METADATA_TEMPLATE_ITEM_TYPE_UNKNOWN;

    if ( type == "STRING" )
    {
        ret = METADATA_TEMPLATE_ITEM_TYPE_STRING;
    }
    else if ( type == "DROPDOWN" )
    {
        ret = METADATA_TEMPLATE_ITEM_TYPE_DROPDOWN;
    }
    else if ( type == "DATETIME" )
    {
        ret = METADATA_TEMPLATE_ITEM_TYPE_DATETIME;
    }

    return ret;
}

//----------------------------------------------------------------------------
/*! \fn     DMetadataTemplateItem::Read()
 *
 *  Return type
 */

bool DMetadataTemplateItem::Read( DMetadataTemplateItem& item, QDomElement& element )
{
    ERROR_F( "DMetadataTemplateItem::Read" );
    
    item = DMetadataTemplateItem();

    // Read-only?
    if ( element.hasAttribute("readonly") && element.attribute("readonly") != "false" && element.attribute("readonly") != "FALSE" && element.attribute("readonly") != "0" )
    {
        item.setReadOnly( true );
    }

    // Visible?
    if ( element.hasAttribute("visible") && (element.attribute("visible") == "false" || element.attribute("visible") == "FALSE" || element.attribute("visible") == "0") )
    {
        item.setVisible( false );
    }

    // Set width
    if ( element.hasAttribute("width") )
    {
        if ( !QVariant(element.attribute("width")).canConvert(QMetaType::UInt) || element.attribute("width") < 0 )
        {
            error << ERRerror << "Invalid width: " << element.attribute("width") << endl;
            return false;
        }

        unsigned int pixels = element.attribute("width").toUInt();
        item.setWidth( pixels );
    }

    // Set font size
    if ( element.hasAttribute("fontsize") )
    {
        if ( !QVariant(element.attribute("fontsize")).canConvert(QMetaType::Int) || element.attribute("fontsize") <= 0 )
        {
            error << ERRerror << "Invalid fontsize: " << element.attribute("fontsize") << endl;
            return false;
        }

        unsigned int pixels = element.attribute("fontsize").toInt();
        item.setFontSize( pixels );
    }

    // Set label font size
    if ( element.hasAttribute("labelfontsize") )
    {
        if ( !QVariant(element.attribute("labelfontsize")).canConvert(QMetaType::Int) || element.attribute("labelfontsize") <= 0 )
        {
            error << ERRerror << "Invalid labelfontsize: " << element.attribute("labelfontsize") << endl;
            return false;
        }

        unsigned int pixels = element.attribute("labelfontsize").toInt();
        item.setLabelFontSize( pixels );
    }

    QDomNode entries = element.firstChild();
    while ( !entries.isNull() )
    {
        QDomElement data = entries.toElement();
        QString tagName = data.tagName();

        if ( tagName == "name" )
        {                   
            item.setName( data.text().toStdString() );
        }
        else if ( tagName == "description" )
        {
            item.setDescription( data.text().toStdString() );
        }
        else if ( tagName == "default" )
        {
            item.setDefaultValue( data.text().toStdString() );
        }
        else if ( tagName == "data" )
        {
            QString dataStr;
            QTextStream stream( &dataStr );
            data.save( stream, 0 );
            item.setData( dataStr.toStdString() );
        }
        else if ( tagName == "type" )
        {
            unsigned int itemType = DMetadataTemplateItem::GetType( data.text().toStdString() );

            if ( itemType == DMetadataTemplateItem::METADATA_TEMPLATE_ITEM_TYPE_UNKNOWN )
            {
                error << ERRerror << "Invalid item type: " << itemType << endl;
                return false;
            }

            item.setType( itemType );
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
