#ifndef DMETADATAITEMGROUPLIST_H
#define DMETADATAITEMGROUPLIST_H
/*****************************************************************************
**
**  Metadata item group list class
**
**  Creation date:  2017-11-30
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
#include    "derror.h"

//  SYSTEM INCLUDES
//

//============================================================================
// CLASS: DMetadataItemGroupList

class DMetadataItemGroupList
{
public:
    DMetadataItemGroupList();
    ~DMetadataItemGroupList();

    unsigned int groupCount() const;
    bool         isValid() const;
    bool         getGroup( DMetadataItemGroup& item, unsigned int index ) const;
    bool         write( std::ostream& out, bool header ) const;

    void         clear();
    void         addGroup( const DMetadataItemGroup& group );

    static bool  Read( DMetadataItemGroupList& list, QDomElement& element );

private:
    std::vector<DMetadataItemGroup> m_MetadataGroupList;
};

//----------------------------------------------------------------------------
// INLINE: DMetadataItemGroupList


//====================================EOF=====================================

#endif  /* DMETADATAITEMGROUPLIST_H */
