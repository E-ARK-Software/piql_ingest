#ifndef DMETADATAITEMLIST_H
#define DMETADATAITEMLIST_H
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
#include    "dmetadataitem.h"

//  BASE INCLUDES
//
#include    "derror.h"

//  SYSTEM INCLUDES
//

//============================================================================
// CLASS: DMetadataItemList

class DMetadataItemList
{
public:
    DMetadataItemList();
    ~DMetadataItemList();

    unsigned int count() const;
    bool         isValid() const;
    bool         getItem( DMetadataItem& item, unsigned int index ) const;
    bool         getItem( DMetadataItem& item, const std::string& key ) const;
    bool         write( std::ostream& out, bool header ) const;

    void         clear();
    void         addItem( const DMetadataItem& item );

    static bool  Read( DMetadataItemList& list, QDomElement& element );

private:
    std::vector<DMetadataItem> m_MetadataList;
};

//----------------------------------------------------------------------------
// INLINE: DMetadataItemList


//====================================EOF=====================================

#endif  /* DMETADATAITEMLIST_H */
