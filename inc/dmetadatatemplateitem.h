#ifndef DMETADATATEMPLATEITEM_H
#define DMETADATATEMPLATEITEM_H

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

//  BASE INCLUDES
//

//  SYSTEM INCLUDES
//
#include <string>
#include <vector>

//  FORWARD DECLARATIONS
//
class QDomElement;

//============================================================================
// CLASS: DMetadataTemplateItem

class DMetadataTemplateItem
{
public:
    enum metadata_template_item_type
    {
        METADATA_TEMPLATE_ITEM_TYPE_UNKNOWN,
        METADATA_TEMPLATE_ITEM_TYPE_STRING,
        METADATA_TEMPLATE_ITEM_TYPE_DROPDOWN,
        METADATA_TEMPLATE_ITEM_TYPE_DATETIME
    };

public:
    DMetadataTemplateItem();
    DMetadataTemplateItem(
        const std::string& name,
        const std::string& description,
        const std::string& defaultValue,
        const std::string& data,
        unsigned int type,
        bool readOnly,
        bool visible,
        int fontSize,
        int labelFontSize,
        unsigned int width = 200 );
    ~DMetadataTemplateItem();

    std::string  name() const;
    std::string  description() const;
    std::string  defaultValue() const;
    std::string  data() const;
    unsigned int type() const;
    bool         readOnly() const;
    bool         visible() const;
    unsigned int width() const;
    int          fontSize() const;
    int          labelFontSize() const;
    bool getSetting( std::string& value, const std::string& key ) const;
    bool getSetting( bool& value, const std::string& key ) const;
    bool getOptions( std::vector<std::string>& names, std::vector<std::string>& values ) const;
    bool validate( const std::string& value ) const;

    void setName( const std::string& name );
    void setDescription( const std::string& description );
    void setDefaultValue( const std::string& defaultValue );
    void setData( const std::string& data );
    void setType( unsigned int type );
    void setReadOnly( bool readOnly );
    void setVisible( bool visible );
    void setWidth( unsigned int width );
    void setFontSize( int fontSize );
    void setLabelFontSize(int fontSize);

    static unsigned int GetType( const std::string& type );
    static bool         Read( DMetadataTemplateItem& item, QDomElement& element );

private:
    std::string  m_Name;
    std::string  m_Description;
    std::string  m_DefaultValue;
    std::string  m_Data;
    unsigned int m_Type;
    bool         m_ReadOnly;
    bool         m_Visible;
    unsigned int m_Width;
    int          m_FontSize;
    int          m_LabelFontSize;
};

//----------------------------------------------------------------------------
// INLINE: DMetadataTemplateItem


//====================================EOF=====================================

#endif  /* DMETADATATEMPLATEITEM_H */
