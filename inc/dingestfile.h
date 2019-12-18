#ifndef DINGESTFILE_H
#define DINGESTFILE_H

/*****************************************************************************
**
**  Configuration class
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
#include    "dmetadataitemgrouplist.h"

//  BASE INCLUDES
//
#include    "derror.h"

//  SYSTEM INCLUDES
//
#include    <string>

//============================================================================
// CLASS: DIngestFile

class DIngestFile
{
public:
    enum Type
    {
        TYPE_UNDEFINED,
        TYPE_FILE,
        TYPE_DIRECTORY
    };

    enum
    {
        FILE_ID_UNDEFINED = -1,
        FILE_ID_NONE = -2
    };

public:
    DIngestFile();
    DIngestFile( const std::string& fileName );
    DIngestFile( const std::string& fileName, DIngestFile::Type type, int id, int parentId );
    ~DIngestFile();

    bool                          isValid() const;
    std::string                   fileName( bool fullPath ) const;
    DIngestFile::Type             type() const;
    std::string                   typeAsString() const;
    int                           id() const;
    int                           parentId() const;
    bool                          hasParent() const;
    const DMetadataItemGroupList& metadataGroupList() const;
    bool                          write( std::ostream& out, bool header ) const;
    void                          setMetadataGroups( const DMetadataItemGroupList& metadataGroupList );
    void                          setFileName( const std::string& name );
    void                          setId( int id );
    void                          setParentId( int id );
    void                          setType( DIngestFile::Type );
                                  
    static bool                   Read( DIngestFile& item, QDomElement& element );

private:
    std::string            m_FileName;
    DMetadataItemGroupList m_MetadataGroupList;
    DIngestFile::Type      m_Type;
    int                    m_Id;
    int                    m_ParentId;
};

//----------------------------------------------------------------------------
// INLINE: DIngestFile


//====================================EOF=====================================

#endif /* DINGESTFILE_H */
