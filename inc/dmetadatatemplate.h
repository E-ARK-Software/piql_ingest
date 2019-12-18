#ifndef DMETADATATEMPLATE_H
#define DMETADATATEMPLATE_H

/*****************************************************************************
**
**  Configuration class
**
**  Creation date:  2017-04-03
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
#include "dmetadatatemplateitemgrouplist.h"

//  BASE INCLUDES
//

//  SYSTEM INCLUDES
//
#include <string>

// FORWARD DECLARATIONS
//
#include <QDomDocument>

//============================================================================
// CLASS: DMetadataTemplate

class DMetadataTemplate
{
public:
    DMetadataTemplate();
    ~DMetadataTemplate();

    bool read( const std::string& fileName );
    bool read( QDomDocument& doc );
    bool read( std::istream& definition );
    const DMetadataTemplateItemList& collectionMetadata() const;
    const DMetadataTemplateItemGroupList& fileMetadataGroups() const;

private:
    DMetadataTemplateItemList m_CollectionMetadata;
    DMetadataTemplateItemGroupList m_FileMetadataGroups;
};

//----------------------------------------------------------------------------
// INLINE: DMetadataTemplate


//====================================EOF=====================================

#endif  /* DMETADATATEMPLATE_H */
