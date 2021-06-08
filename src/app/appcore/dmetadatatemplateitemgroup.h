#ifndef DMETADATATEMPLATEITEMGROUP_H
#define DMETADATATEMPLATEITEMGROUP_H

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
#include "dmetadatatemplateitemlist.h"

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
// CLASS: DMetadataTemplateItemGroup

class DMetadataTemplateItemGroup
{
public:
    DMetadataTemplateItemGroup();
    DMetadataTemplateItemGroup( const std::string& name, const std::string& description );
    ~DMetadataTemplateItemGroup();

    std::string  name() const;
    std::string  description() const;
    unsigned int itemCount() const;

    void setName( const std::string& name );
    void setDescription( const std::string& description );
    void setItems( const DMetadataTemplateItemList& items );
    bool getItem( DMetadataTemplateItem& item, unsigned int index ) const;

    static bool Read( DMetadataTemplateItemGroup& group, QDomElement& element );

private:
    std::string               m_Name;
    std::string               m_Description;
    DMetadataTemplateItemList m_ItemList;
};

//----------------------------------------------------------------------------
// INLINE: DMetadataTemplateItemGroup


//====================================EOF=====================================

#endif  /* DMETADATATEMPLATEITEMGROUP_H */
