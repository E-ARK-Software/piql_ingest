#ifndef DMETADATATEMPLATEITEMGROUPLIST_H
#define DMETADATATEMPLATEITEMGROUPLIST_H

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
#include "dmetadatatemplateitemgroup.h"

//  BASE INCLUDES
//

//  SYSTEM INCLUDES
//
#include <string>
#include <vector>

//============================================================================
// CLASS: DMetadataTemplateItemGroupList

class DMetadataTemplateItemGroupList
{
public:
    DMetadataTemplateItemGroupList();
    ~DMetadataTemplateItemGroupList();

    void clear();
    bool addGroup( const DMetadataTemplateItemGroup& group );

    unsigned int groupCount() const;
    bool getGroup( DMetadataTemplateItemGroup& group, unsigned int index ) const;

    static bool Read( DMetadataTemplateItemGroupList& groupList, QDomElement& element );

private:
    std::vector<DMetadataTemplateItemGroup> m_GroupList;
};

//----------------------------------------------------------------------------
// INLINE: DMetadataTemplateItemGroupList


//====================================EOF=====================================

#endif  /* DMETADATATEMPLATEITEMGROUPLIST_H */
