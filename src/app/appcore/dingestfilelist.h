#ifndef DINGESTFILELIST_H
#define DINGESTFILELIST_H

/*****************************************************************************
**
**  Configuration class
**
**  Creation date:  2017-03-30
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
#include    "dingestfile.h"

//  BASE INCLUDES
//
#include    "derror.h"

//  SYSTEM INCLUDES
//
#include    <vector>

//============================================================================
// CLASS: DIngestFileList

class DIngestFileList
{
public:
    DIngestFileList();
    ~DIngestFileList();

    unsigned int count() const;
    void         clear();
    unsigned int addFile( const DIngestFile& file );
    bool         addFileFromDisk( std::vector<int>& ids, const std::string& fileName, int parentId = DIngestFile::FILE_ID_NONE );
    bool         getFileByIndex( DIngestFile& ingestFile, unsigned int index ) const;
    bool         getFileById( DIngestFile& ingestFile, int id ) const;
    bool         getRelativePath( std::string& path, const DIngestFile& ingestFile ) const;
    bool         setFileByIndex( const DIngestFile& ingestFile, unsigned int index );
    bool         setFileById( const DIngestFile& ingestFile, int id );
    bool         setFilesByIndex( const DIngestFileList& ingestFiles, std::vector< unsigned int > indexes );
    bool         setFilesById(const DIngestFileList& ingestFiles, std::vector< int > indexes);
    bool         removeFileByIndex( unsigned int index );
    bool         removeFileById( int id );
    bool         write( std::ostream& out, bool header ) const;
    bool         isValid() const;
    bool         idExists( int id ) const;

    static bool Read( DIngestFileList& itemList, QDomElement& element );

private:
    int          generateId() const;
    unsigned int idToIndex( int id ) const;

private:
    std::vector<DIngestFile> m_FileList;
};

//----------------------------------------------------------------------------
// INLINE: DIngestFileList


//====================================EOF=====================================

#endif  /* DINGESTFILELIST_H */
