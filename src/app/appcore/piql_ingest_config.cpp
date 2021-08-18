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
#include    "piql_ingest_config.h"

//  QT INCLUDES
//
#include    <QStandardPaths>
#include    <QDir>

//  SYSTEM INCLUDES
//

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )


/****************************************************************************/
/*! \class DPiqlIngestConfig piql_ingest_config.h
 *  \ingroup PiqlIngest
 *  \brief DIngest Tool config file class.
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================


//----------------------------------------------------------------------------
/*! \fn     DPiqlIngestConfig::DPiqlIngestConfig()
 *
 *  Load config values from file
 */

DPiqlIngestConfig::DPiqlIngestConfig( const std::string& filename )
{
    QSettings settings( QString::fromStdString(filename), QSettings::IniFormat );

    m_LogLevel = DError::StringToType( settings.value("LOG_LEVEL", "error").toString().toStdString() );
    m_DeleteTempFiles = settings.value( "DELETE_TEMP_FILES", true ).toBool();
    m_HideConsoleWindow = settings.value( "HIDE_CONSOLE_WINDOW", true ).toBool();
    m_AllowEmptyMetadata = settings.value( "ALLOW_EMPTY_METADATA", false ).toBool();
    m_AutoOpenMetadataEdit = settings.value( "AUTO_OPEN_METADATA_EDIT", true ).toBool();
    m_AutoOpenPackageMetadataEdit = settings.value("AUTO_OPEN_PACKAGE_METADATA_EDIT", false).toBool();
    m_CommitSinglePackage = settings.value( "COMMIT_SINGLE_PACKAGE", false ).toBool();
    m_ApplicationStyleSheet = settings.value( "APPLICATION_STYLE_SHEET", "").toString().toStdString();
    m_TempFilesDirectory = settings.value("TEMP_FILES_DIRECTORY", QStandardPaths::writableLocation(QStandardPaths::TempLocation)).toString().append(QDir::separator()).append("piql_ingest").toLocal8Bit().toStdString();
    m_UserDataDirectory = settings.value("USER_DATA_DIRECTORY", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).toString().append(QDir::separator()).toLocal8Bit().toStdString();
    m_ReportsDirectory = settings.value("REPORTS_DIRECTORY", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).toString().append(QDir::separator()).toLocal8Bit().toStdString();
    m_ValidateFilenameRegex = settings.value("VALIDATE_FILENAME_REGEX", "[^ ]+").toString().toStdString();
    m_MaxBundleSize = settings.value("MAX_BUNDLE_SIZE", 1).toInt();
    m_PrimaryLogo = settings.value("PRIMARY_LOGO", "").toString().toStdString();
    m_SecondaryLogo = settings.value("SECONDARY_LOGO", "").toString().toStdString();
    m_RightLogo = settings.value("RIGHT_LOGO", "").toString().toStdString();
    m_MainWindowTitle = settings.value("MAIN_WINDOW_TITLE", "").toString().toStdString();
    m_MainWindowIcon = settings.value("MAIN_WINDOW_ICON", "").toString().toStdString();
    m_ApplicationLanguage = settings.value("APPLICATION_LANGUAGE", "").toString().toStdString();
    m_CommitButtonText = settings.value("COMMIT_BUTTON_TEXT", "").toString().toStdString();
}


//----------------------------------------------------------------------------
/*! 
 *  Log level
 */

DError::Type DPiqlIngestConfig::logLevel()
{
    return m_LogLevel;
}

//----------------------------------------------------------------------------
/*!
*  Delete temp files produced by application
*/

bool DPiqlIngestConfig::deleteTempFiles()
{
    return m_DeleteTempFiles;
}

//----------------------------------------------------------------------------
/*!
*  Hide the console window at application startup on W32
*/

bool DPiqlIngestConfig::hideConsoleWindow()
{
    return m_HideConsoleWindow;
}

//----------------------------------------------------------------------------
/*!
*  Allow metadata fields to be left empty
*/

bool DPiqlIngestConfig::allowEmptyMetadata()
{
    return m_AllowEmptyMetadata;
}

//----------------------------------------------------------------------------
/*!
 *  Automatically open metadata edit dialog when adding files
 */

bool DPiqlIngestConfig::autoOpenMetadataEdit()
{
    return m_AutoOpenMetadataEdit;
}

//----------------------------------------------------------------------------
/*!
 *  Automatically open package metadata edit dialog before sending files to sftp server
 */

bool DPiqlIngestConfig::autoOpenPackageMetadataEdit()
{
    return m_AutoOpenPackageMetadataEdit;
}

//----------------------------------------------------------------------------
/*!
*  Commit all marked files in the same package - otherwise one package per file
*/

bool DPiqlIngestConfig::commitSinglePackage()
{
    return m_CommitSinglePackage;
}

//----------------------------------------------------------------------------
/*!
*  Style sheet to be applied to application - CSS style
*/

std::string DPiqlIngestConfig::applicationStyleSheet()
{
    return m_ApplicationStyleSheet;
}

//----------------------------------------------------------------------------
/*!
*  Path of temporary files
*/

std::string DPiqlIngestConfig::tempFilesDirectory()
{
    return m_TempFilesDirectory;
}

//----------------------------------------------------------------------------
/*!
*  Path of user data
*/

std::string DPiqlIngestConfig::userDataDirectory()
{
    return m_UserDataDirectory;
}

//----------------------------------------------------------------------------
/*!
*  Path of reports
*/

std::string DPiqlIngestConfig::reportsDirectory()
{
    return m_ReportsDirectory;
}

//----------------------------------------------------------------------------
/*!
*  Regular expression for validating name of files added to list
*/

std::string DPiqlIngestConfig::validateFilenameRegex()
{
    return m_ValidateFilenameRegex;
}

//----------------------------------------------------------------------------
/*!
*  Maximum number of files to be bundled into a compound SIP
*/

unsigned int DPiqlIngestConfig::maxBundleSize()
{
    return m_MaxBundleSize;
}

//----------------------------------------------------------------------------
/*!
*  Path of primary logo
*/

std::string DPiqlIngestConfig::primaryLogo()
{
    return m_PrimaryLogo;
}

//----------------------------------------------------------------------------
/*!
*  Path of secondary logo
*/

std::string DPiqlIngestConfig::secondaryLogo()
{
    return m_SecondaryLogo;
}

//----------------------------------------------------------------------------
/*!
*  Path of right logo
*/

std::string DPiqlIngestConfig::rightLogo()
{
    return m_RightLogo;
}

//----------------------------------------------------------------------------
/*!
*  Main window title
*/

std::string DPiqlIngestConfig::mainWindowTitle()
{
    return m_MainWindowTitle;
}

//----------------------------------------------------------------------------
/*!
*  Main window icon
*/

std::string DPiqlIngestConfig::mainWindowIcon()
{
    return m_MainWindowIcon;
}

//----------------------------------------------------------------------------
/*!
*  Application language
*/

std::string DPiqlIngestConfig::applicationLanguage()
{
    return m_ApplicationLanguage;
}

//----------------------------------------------------------------------------
/*!
*  Commit button text
*/

std::string DPiqlIngestConfig::commitButtonText()
{
    return m_CommitButtonText;
}
