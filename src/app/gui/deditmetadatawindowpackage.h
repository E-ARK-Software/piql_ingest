#ifndef DEDITMETADATAWINDOWPACKAGE_H
#define DEDITMETADATAWINDOWPACKAGE_H

/*****************************************************************************
**
**  Definition of the DEditMetadataWindowPackage class
**
**  Creation date:  2019/11/25
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
// CLASS: DEditMetadataWindowPackage
class DEditMetadataWindowPackage : public DEditMetadataWindowBase
{
    Q_OBJECT

public:
    DEditMetadataWindowPackage(QWidget * parent, DMetadataItemGroupList& metadataGroupList, const std::string& phpBinPath, const DPath& tempDir, DPiqlIngestConfig * config);
    ~DEditMetadataWindowPackage();

    void showForm();
    bool validate() const;

protected:
    bool FillStackedWidget(const std::string& phpBinPath);

private slots:
    void saveButtonPressed();

private:
    DMetadataItemGroupList& m_MetadataGroupList;
};

D_NAMESPACE_END

#endif /* DEDITMETADATAWINDOWPACKAGE_H */
