#ifndef DMETADATATEMPLATEITEMLIST_H
#define DMETADATATEMPLATEITEMLIST_H

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
#include "dmetadatatemplateitem.h"

//  BASE INCLUDES
//

//  SYSTEM INCLUDES
//
#include <string>
#include <vector>

//============================================================================
// CLASS: DMetadataTemplateItemList

class DMetadataTemplateItemList
{
public:
    DMetadataTemplateItemList();
    ~DMetadataTemplateItemList();

    void clear();
    bool addItem( const DMetadataTemplateItem& item );

    unsigned int count() const;
    bool getItem( DMetadataTemplateItem& item, unsigned int index ) const;

    static bool Read( DMetadataTemplateItemList& itemList, QDomElement& element );

private:
    std::vector<DMetadataTemplateItem> m_ItemList;
};

//----------------------------------------------------------------------------
// INLINE: DMetadataTemplateItemList


//====================================EOF=====================================

#endif  /* DMETADATATEMPLATEITEMLIST_H */
