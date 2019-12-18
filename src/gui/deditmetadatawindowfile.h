#ifndef DEDITMETADATAWINDOWFILE_H
#define DEDITMETADATAWINDOWFILE_H

/*****************************************************************************
**
**  Definition of the DEditMetadataWindowBase class
**
**  Creation date:  2019/11/19
**  Created by:     Oleksandr Ivanov
**
**
**  Copyright (c) 2019 Piql AS. All rights reserved.
**
**  This file is part of the ingest tool application
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "deditmetadatawindowbase.h"

//  NAMESPACE
//
D_NAMESPACE_BEGIN(D_NAMESPACE)

//============================================================================
// CLASS: DEditMetadataWindowFile
class DEditMetadataWindowFile : public DEditMetadataWindowBase
{
    Q_OBJECT

public:
    DEditMetadataWindowFile(QWidget* parent, DIngestFileList& ingestFiles, const std::string& phpBinPath, const DPath& tempDir, DPiqlIngestConfig* config);
    ~DEditMetadataWindowFile();

    void showForm();
    bool validate() const;

protected:
    bool FillStackedWidget(const std::string& phpBinPath);

private slots:
    void saveButtonPressed();
    void navigatePrevious();
    void navigateNext();
    void navigateEnd();

private:
    DIngestFileList& m_IngestFiles;
};

D_NAMESPACE_END

#endif /* DEDITMETADATAWINDOWFILE_H */
