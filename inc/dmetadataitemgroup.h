#ifndef DMETADATAITEMGROUP_H
#define DMETADATAITEMGROUP_H

/*****************************************************************************
**
**  Metadata item group class
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
#include     "dmetadataitemlist.h"

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
// CLASS: DMetadataItemGroup

class DMetadataItemGroup
{
public:
    DMetadataItemGroup();
    DMetadataItemGroup( const std::string& name );
    ~DMetadataItemGroup();

    bool isValid() const;
    bool write( std::ostream& out ) const;
    void setName( const std::string& name );
    void addItem( const DMetadataItem& item );
    bool getItem( DMetadataItem& item, unsigned int index ) const;
    bool getItem( DMetadataItem& item, const std::string& key ) const;
    void clear();

    std::string name() const;
    unsigned int itemCount() const;

    static bool Read( DMetadataItemGroup& group, QDomElement& element );

private:
    std::string       m_Name;
    DMetadataItemList m_MetadataItems;
};

//----------------------------------------------------------------------------
// INLINE: DMetadataItemGroup


//====================================EOF=====================================

#endif /* DMETADATAITEMGROUP_H */
