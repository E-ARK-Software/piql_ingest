#ifndef PIQLINGESTCONFIG_H
#define PIQLINGESTCONFIG_H

/*****************************************************************************
**
**  Configuration class
**
**  Creation date:  2017-03-29
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

//  BASE INCLUDES
//
#include    "derror.h"

//  QT INCLUDES
//
#include    <QSettings>

//  SYSTEM INCLUDES
//

//============================================================================
// CLASS: DPiqlIngestConfig
class DPiqlIngestConfig
{
public:

    DPiqlIngestConfig( const std::string& filename );

    D_NAMESPACE::DError::Type   logLevel();
    bool                        deleteTempFiles();
    bool                        hideConsoleWindow();
    bool                        allowEmptyMetadata();
    bool                        autoOpenMetadataEdit();
    bool                        autoOpenPackageMetadataEdit();
    bool                        commitSinglePackage();
    bool                        enableFileMetadataEdit();
    bool                        showAddFilesButton();
    bool                        autofillLastInputPackageMetadata();
    unsigned int                maxBundleSize();
    std::string                 applicationStyleSheet();
    std::string                 tempFilesDirectory();
    std::string                 userDataDirectory();
    std::string                 reportsDirectory();
    std::string                 validateFilenameRegex();
    std::string                 primaryLogo();
    std::string                 secondaryLogo();
    std::string                 rightLogo();
    std::string                 mainWindowTitle();
    std::string                 mainWindowIcon();
    std::string                 applicationLanguage();
    std::string                 commitButtonText();

private:

    D_NAMESPACE::DError::Type   m_LogLevel;
    bool                        m_DeleteTempFiles;
    bool                        m_HideConsoleWindow;
    bool                        m_AllowEmptyMetadata;
    bool                        m_AutoOpenMetadataEdit;
    bool                        m_AutoOpenPackageMetadataEdit;
    bool                        m_CommitSinglePackage;
    bool                        m_EnableFileMetadataEdit;
    bool                        m_ShowAddFilesButton;
    bool                        m_AutofillLastInputPackageMetadata;
    unsigned int                m_MaxBundleSize;
    std::string                 m_ApplicationStyleSheet;
    std::string                 m_TempFilesDirectory;
    std::string                 m_UserDataDirectory;
    std::string                 m_ReportsDirectory;
    std::string                 m_ValidateFilenameRegex;
    std::string                 m_PrimaryLogo;
    std::string                 m_SecondaryLogo;
    std::string                 m_RightLogo;
    std::string                 m_MainWindowTitle;
    std::string                 m_MainWindowIcon;
    std::string                 m_ApplicationLanguage;
    std::string                 m_CommitButtonText;

private: // DISABLE
    
    DPiqlIngestConfig( const DPiqlIngestConfig& );
    DPiqlIngestConfig& operator = ( const DPiqlIngestConfig& );
};

//----------------------------------------------------------------------------
// INLINE: DPiqlIngestConfig


//====================================EOF=====================================

#endif  /* PIQLINGESTCONFIG_H */
