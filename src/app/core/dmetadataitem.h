#ifndef DMETADATAITEM_H
#define DMETADATAITEM_H

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

//  BASE INCLUDES
//
#include    "derror.h"

//  SYSTEM INCLUDES
//
#include    <string>

//  FORWARD DECLARATIONS
//
class QDomElement;

//============================================================================
// CLASS: DMetadataItem

class DMetadataItem
{
public:
    DMetadataItem();
    DMetadataItem( const std::string& key, const std::string& value );
    ~DMetadataItem();

    bool isValid() const;
    bool write( std::ostream& out ) const;
    void setKey( const std::string& key );
    void setValue( const std::string& value );

    std::string key() const;
    std::string value() const;

    static bool Read( DMetadataItem& item, QDomElement& element );

private:
    std::string m_Key;
    std::string m_Value;
};

//----------------------------------------------------------------------------
// INLINE: DMetadataItem


//====================================EOF=====================================

#endif /* DMETADATAITEM_H */
