/***************************************************************************
**
**  Implementation of the DPiqlIngestMainWindow class
**
**  Creation date:  2017/03/29
**  Created by:     Matti Wennberg
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the ingest tool application
**
***************************************************************************/

//  PROJECT INCLUDES
//
#include    "dpiqlingestmainwindow.h"
#include    "dmetadatatemplate.h"
#include    "deditmetadatawindowfile.h"
#include    "deditmetadatawindowpackage.h"
#include    "piql_ingest_config.h"
#include    "dphputils.h"
#include    "dscriptcontext.h"

//  BASE INCLUDES
//
#include    "derror.h"
#include    "dqterror.h"
#include    "qpersistantfiledialog.h"
#include    "dstringtools.h"
#include    "dfilesystemtools.h"
#include    "dsystemtools.h"
#include    "dregexp.h"

//  QT INCLUDES
//
#include    <QMessageBox>
#include    <QMimeData>
#include    <QTextStream>

//  SYSTEM INCLUDES
//
#include    <fstream>

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )

/****************************************************************************/
/*! \class DPiqlIngestMainWindow dpiqlingestmainwindow.h
 *  \ingroup PiqlIngest
 *  \brief Dialog for controlling the piql_ingest using a GUI
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! 
 *
 */

 DPiqlIngestMainWindow::DPiqlIngestMainWindow( DPiqlIngestConfig* config )
  : m_Config( config ),
    m_Transaction( 0 ),
    m_TempDir( config->tempFilesDirectory(), "" ),
    m_DataDir( config->userDataDirectory(), "" ),
    m_ReportsDir( config->reportsDirectory(), "" ),
    m_CommitCanceled( false )
{
    ERROR_F( "DPiqlIngestMainWindow::DPiqlIngestMainWindow" );
    
    m_Ui.setupUi( this );

    // Cleanup temp directory
    if ( !cleanup() )
    {
        showErrorMessage( QMessageBox::tr("Failed to delete files"), QMessageBox::tr("Failed to delete temporary files") );
        error << ERRerror << "Failed to delete temporary files" << endl;
    }

    // Load application context
    if ( !loadContext() )
    {
        showErrorMessage( QMessageBox::tr("Failed to load context"), QMessageBox::tr("Failed to load context") );
        error << ERRerror << "Failed to load context" << endl;
        close();
    }

    // Set primary logo
    if ( !m_Config->primaryLogo().empty() )
    {
        QPixmap pixmap( QString::fromStdString(m_Config->primaryLogo()) );
        if ( pixmap.isNull() )
        {
            error << ERRerror << "Failed to load primary logo: " << m_Config->primaryLogo() << endl;
        }
        else
        {
            m_Ui.primaryLogo->setPixmap( pixmap );
            m_Ui.primaryLogo->setFixedWidth( pixmap.width() );
            m_Ui.primaryLogo->setFixedHeight( pixmap.height() );
        }
    }
    
    // Set secondary logo
    if ( !m_Config->secondaryLogo().empty() )
    {
        QPixmap pixmap( QString::fromStdString(m_Config->secondaryLogo()) );
        if ( pixmap.isNull() )
        {
            error << ERRerror << "Failed to load secondary logo: " << m_Config->secondaryLogo() << endl;
        }
        else
        {
            m_Ui.secondaryLogo->setPixmap( pixmap );
            m_Ui.secondaryLogo->setFixedWidth( pixmap.width() );
            m_Ui.secondaryLogo->setFixedHeight( pixmap.height() );
        }
    }

    // Set right logo
    if ( !m_Config->rightLogo().empty() )
    {
        QPixmap pixmap( QString::fromStdString(m_Config->rightLogo()) );
        if ( pixmap.isNull() )
        {
            error << ERRerror << "Failed to load right logo: " << m_Config->rightLogo() << endl;
        }
        else
        {
            m_Ui.rightLogo->setPixmap( pixmap );
            m_Ui.rightLogo->setFixedWidth( pixmap.width() );
            m_Ui.rightLogo->setFixedHeight( pixmap.height() );
        }
    }
    
    // The progress dialog is used during time consuming operations
    m_ProgressDialog = NULL;

    // Set window title
    if ( !m_Config->mainWindowTitle().empty() )
    {
        setWindowTitle( QString::fromStdString(m_Config->mainWindowTitle()) );
    }
    else
    {
        setWindowTitle( "Piql Ingest" );
    }

    // Set commit button icon
    string commitButtonText = m_Config->commitButtonText();
    if ( commitButtonText.empty() )
    {
        QPixmap commitButtonPixmap( ":/images/gui/resources/commit_button.png" );
        QIcon commitButtonIcon( commitButtonPixmap );
        m_Ui.commitButton->setIcon( commitButtonIcon );
        m_Ui.commitButton->setIconSize( commitButtonPixmap.rect().size() );
        m_Ui.commitButton->setFixedSize( commitButtonPixmap.rect().size() );
    }
    else
    {
        const int padding = 10;
        m_Ui.commitButton->setText( QString::fromStdString(commitButtonText) );
        m_Ui.commitButton->setFixedSize( m_Ui.commitButton->sizeHint().width() + padding, m_Ui.commitButton->sizeHint().height() + padding );
    }

    // Set window icon
    if ( !m_Config->mainWindowIcon().empty() )
    {
        setWindowIcon( QIcon(QString::fromStdString(m_Config->mainWindowIcon())) );
    }
    
    // Make tree multi-select
    m_Ui.treeWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );

    // Set information view to be read-only
    m_Ui.informationView->setReadOnly( true );

    // Set size of button boxes
    QSizePolicy sizePolicy = m_Ui.placeHolderButton->sizePolicy();
    sizePolicy.setRetainSizeWhenHidden( true );
    m_Ui.placeHolderButton->setSizePolicy( sizePolicy );
    m_Ui.placeHolderButton->setVisible( false );

    // Setup which buttons should be visible
    m_Ui.editMetadataButton->setVisible( m_Config->enableFileMetadataEdit() );

    // Accept dropping files on main window
    setAcceptDrops( true );

    // Signals and slots. The GUI components emits signals that are handled by the slots.
    QObject::connect( m_Ui.addFilesButton, SIGNAL(clicked()), this, SLOT(selectFilesButtonPressed()) );
    QObject::connect( m_Ui.deleteFilesButton, SIGNAL(clicked()), this, SLOT(deleteFilesButtonPressed()) );
    QObject::connect( m_Ui.editMetadataButton, SIGNAL(clicked()), this, SLOT(editMetadataButtonPressed()) );
    QObject::connect( m_Ui.commitButton, SIGNAL(clicked()), this, SLOT(commitButtonPressed()) );
    QObject::connect( m_Ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(editMetadataButtonPressed()) );
    QObject::connect( m_Ui.treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(activatedTreeItem(QTreeWidgetItem*,int)) );
    QObject::connect( m_Ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(activatedTreeItem(QTreeWidgetItem*,int)) );
    QObject::connect( m_Ui.treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(activatedTreeItem()) );

    // Update GUI
    updateGui();
}


//----------------------------------------------------------------------------
/*! 
 *
 */

DPiqlIngestMainWindow::~DPiqlIngestMainWindow()
{
    ERROR_F( "DPiqlIngestMainWindow::~DPiqlIngestMainWindow" );

    // Cleanup temp directory
    if ( !cleanup() )
    {
        showErrorMessage( "Failed to delete files", "Failed to delete temporary files" );
        error << ERRerror << "Failed to delete temporary files" << endl;
    }
}

//----------------------------------------------------------------------------
/*! 
 *  Show dialog with error message
 */

void DPiqlIngestMainWindow::showErrorMessage( const QString& header, const QString& message )
{
    QMessageBox::critical( this, header, message );
}

//----------------------------------------------------------------------------
/*! 
 *  Show dialog with warning message
 */

void DPiqlIngestMainWindow::showWarningMessage( const QString& header, const QString& message )
{
    QMessageBox::warning( this, header, message );
}

//----------------------------------------------------------------------------
/*! 
 *  Show dialog with info message
 */

void DPiqlIngestMainWindow::showInfoMessage( const QString& header, const QString& message )
{
    QMessageBox::information( this, header, message );
}

//----------------------------------------------------------------------------
/*! 
 *  Accept dragging files over main window
 */

void DPiqlIngestMainWindow::dragEnterEvent( QDragEnterEvent *e )
{
    if ( e->mimeData()->hasUrls() )
    {
        e->acceptProposedAction();
    }
}

//----------------------------------------------------------------------------
/*! 
 *  Accept dragging files over main window
 */

void DPiqlIngestMainWindow::dragMoveEvent( QDragMoveEvent *e )
{
    if ( e->mimeData()->hasUrls() )
    {
        e->acceptProposedAction();
    }
}


//----------------------------------------------------------------------------
/*! 
 *  Accept dropping files over main window
 */


void DPiqlIngestMainWindow::dropEvent( QDropEvent *e )
{
    QStringList fileNames;
    foreach ( const QUrl &url, e->mimeData()->urls() )
    {
        QString fileName = url.toLocalFile();
        fileNames.append( fileName );
    }

    addFiles( fileNames );
}


//----------------------------------------------------------------------------
/*! 
 *  Select files to be uploaded
 */

void DPiqlIngestMainWindow::selectFilesButtonPressed()
{
    // Add multi-file-select
    QStringList fileNames = QPersistantFileDialog::getOpenFileNames( "selectFilesButtonPressed", this, QCoreApplication::translate("QPersistantFileDialog::", "Select files to upload"), ".", "*", 0, QFileDialog::DontUseNativeDialog );

    addFiles( fileNames );
}


vector<int> DPiqlIngestMainWindow::selectedIds( bool recursive /*= false*/, int fileId /*= -1*/ ) const
{
    ERROR_F( "DPiqlIngestMainWindow::selectedIds" );
    
    vector<int> ids;
    if ( fileId == -1 )
    {
        QList<QTreeWidgetItem*> items = m_Ui.treeWidget->selectedItems();
        for ( int i = 0; i < items.size(); i++ )
        {
            int id;
            if ( !m_FileTreeMap.getId(id, items[i]) )
            {
                return vector<int>();
            }
            ids.push_back( id );
        }
    }
    else
    {
        ids.push_back( fileId );
    }

    if ( recursive )
    {
        const unsigned int idsSize = ids.size();
        for ( unsigned int i = 0; i < idsSize; i++ )
        {
            DIngestFile file;
            m_FileList.getFileById(file, ids[i]);
            for ( unsigned int j = 0; j < m_FileList.count(); j++ )
            {
                DIngestFile child;
                m_FileList.getFileByIndex( child, j );
                if ( child.parentId() == file.id() )
                {
                    vector<int> childIds = selectedIds(recursive, child.id());
                    ids.insert( ids.end(), childIds.begin(), childIds.end() );
                }
            }
        }
    }

    return ids;
}


//----------------------------------------------------------------------------
/*! 
 *  Edit metadata for selected file
 */

void DPiqlIngestMainWindow::editMetadataButtonPressed()
{
    ERROR_F( "DPiqlIngestMainWindow::editMetadataButtonPressed" );

    // Get selected files
    bool recursive = true;
    vector<int> selected = selectedIds( recursive );

    // Edit metadata for selected files
    QString errorMessage;
    if ( !editMetadata(errorMessage, selected) )
    {
        error << ERRerror << "Failed to edit metadata" << endl;
        showErrorMessage( QMessageBox::tr("Edit metadata failed"), errorMessage );
        return;
    }
}


//----------------------------------------------------------------------------
/*! 
 *  Delete files button pressed
 */

void DPiqlIngestMainWindow::deleteFilesButtonPressed()
{
    ERROR_F( "DPiqlIngestMainWindow::deleteFilesButtonPressed" );

    // Confirm action
    const bool requireConfirm = true;
    QMessageBox::StandardButton reply = QMessageBox::Ok;
    if ( requireConfirm )
    {
        reply = QMessageBox::question( this, QMessageBox::tr("Delete files"), QMessageBox::tr("Delete the files?"), QMessageBox::Ok|QMessageBox::Cancel );
    }

    // Delete file
    if ( reply != QMessageBox::Ok )
    {
        return;
    }

    // Get selected files
    vector<int> selected = selectedIds();

    // Delete files
    for ( unsigned int i = 0; i < selected.size(); i++ )
    {
        int id = selected[i];
        QString errorMessage;
        if ( !deleteFile( errorMessage, id ) )
        {
            error << ERRerror << "Failed to delete files" << endl;
            showErrorMessage( QMessageBox::tr("Delete files failed"), errorMessage );
            break;
        }

        // Save context
        if ( !saveContext() )
        {
            showErrorMessage(QMessageBox::tr("Error"), QMessageBox::tr("Failed to update context"));
            error << ERRerror << "Failed to update context" << endl;
        }
        
        if ( m_Ui.treeWidget->currentIndex().row() < 0 )
        {
            m_Ui.deleteFilesButton->setDisabled( true );
            m_Ui.editMetadataButton->setDisabled( true );
        }
    }

    // Update GUI
    updateGui();
}


//----------------------------------------------------------------------------
/*! 
 *  Commit button pressed
 */

void DPiqlIngestMainWindow::commitButtonPressed()
{
    ERROR_F( "DPiqlIngestMainWindow::commitButtonPressed" );

    const bool commitSinglePackage = m_Config->commitSinglePackage();
    const unsigned int maxBundleSize = m_Config->maxBundleSize();

    // Get selected files
    bool recursive = true;
    vector<int> selected = selectedIds( recursive );

    // Check for files selected more than once
    if ( hasDuplicates(selected) )
    {
        error << ERRinfo << "Selected files contain duplicates" << endl;
        showInfoMessage(QMessageBox::tr("Commit error"), QMessageBox::tr("Some files are selected more than once"));
        return;
    }

    // Check if file package metadata is required
    bool existenceMetadataPackage = false;

    string metadataPackageTemplateFile = DPhpUtils::GetScriptPath("metadata-description-package.php");
    if (metadataPackageTemplateFile.length() != 0 && m_Config->autoOpenPackageMetadataEdit())
    {
        QString errorMessage;
        
        // Create dialog
        DEditMetadataWindowBase * editMetadataDialog = new DEditMetadataWindowPackage(this, m_PackageMetadata, phpPath(), m_TempDir, m_Config);

        if (!createEditMetadataDialog(errorMessage, editMetadataDialog, m_PackageMetadata))
        {
            error << ERRinfo << "Error creating edit metadata dialog" << endl;
            showInfoMessage(QMessageBox::tr("Edit package metadata dialog"), QString(QMessageBox::tr("%1")).arg(errorMessage.toStdString().c_str()));
            return;
        }

        existenceMetadataPackage = true;
        delete editMetadataDialog;
    }

    m_ProgressDialog = new DPiqlIngestProgressDialog( QProgressDialog::tr("Sending files to server"), QProgressDialog::tr("Cancel"), 0, 100, this );
    m_ProgressDialog->setWindowTitle( QProgressDialog::tr("Sending file to server") );
    m_ProgressDialog->setWindowModality( Qt::WindowModal );
    m_ProgressDialog->setLabelText( QProgressDialog::tr("Starting commit") );
    m_ProgressDialog->resize( 300, 100 );
    m_ProgressDialog->setAutoClose( false );
    m_ProgressDialog->setAutoReset( false );
    connect( m_ProgressDialog, SIGNAL(canceled()), this, SLOT(commitCanceled()) );
    m_ProgressDialog->show();
    QCoreApplication::processEvents();

    vector<int> successfulIds;
    unsigned int sipCount = 0;
    unsigned int totalSipCount = 0;
    for ( unsigned int i = 0; i < selected.size(); i++ )
    {
        sipCount++;

        // Get file index
        vector<int> ids;
        if ( commitSinglePackage )
        {
            ids = selected;
            totalSipCount = 1;
        }
        else
        {
            for ( unsigned int j = 0; j < maxBundleSize && i + j < selected.size(); j++ )
            {
                ids.push_back( selected[i+j] );
            }
            i += ids.size() - 1;

            totalSipCount = selected.size() % maxBundleSize == 0 ? selected.size() / maxBundleSize : selected.size() / maxBundleSize + 1;
        }

        // Update progress dialog title
        m_ProgressDialog->setWindowTitle( QString(QProgressDialog::tr("Sending file %1 of %2")).arg(sipCount).arg(totalSipCount) );

        // Commit files
        QString errorMessage;
        bool canceled = false;
        bool commitOk = commit(canceled, errorMessage, ids, existenceMetadataPackage);
        if ( !writeTransactionReport(m_Transaction - 1, ids, commitOk, canceled) )
        {
            error << ERRerror << "Failed to write report for transaction " << m_Transaction - 1 << endl;
            showErrorMessage(QMessageBox::tr("Error"), QMessageBox::tr("Failed to write report"));
        }
        if ( commitOk )
        {
            successfulIds.insert( successfulIds.end(), ids.begin(), ids.end() );
            m_ProgressDialog->setLabelText(QProgressDialog::tr("Finished successfully"));

            // Remove files from list
            for ( unsigned int j = 0; j < ids.size(); j++ )
            {
                if ( !m_FileList.removeFileById(ids[j]) )
                {
                    showErrorMessage(QMessageBox::tr("Error"), QMessageBox::tr("Failed to update list"));
                    error << ERRerror << "Failed to remove file from list" << endl;
                    break;
                }
            }

            // Save context
            if ( !saveContext() )
            {
                showErrorMessage(QMessageBox::tr("Error"), QMessageBox::tr("Failed to update context"));
                error << ERRerror << "Failed to update context" << endl;
            }

            if ( m_Ui.treeWidget->currentIndex().row() < 0 )
            {
                m_Ui.deleteFilesButton->setDisabled(true);
                m_Ui.editMetadataButton->setDisabled(true);
                m_Ui.commitButton->setDisabled(true);
            }

            // Update GUI
            updateGui();
        }

        // Feedback to user
        if ( i == selected.size() - 1 || commitSinglePackage || canceled )
        {
            if ( canceled )
            {
                error << ERRinfo << "Commit was canceled" << endl;
                showInfoMessage(QMessageBox::tr("Cancel"), QMessageBox::tr("Commit was canceled"));
            }
            else
            {
                if ( successfulIds.size() == selected.size() )
                {
                    showInfoMessage( QMessageBox::tr("Commit successful"), QMessageBox::tr("All files were committed successfully") );
                }
                else if ( successfulIds.size() > 0 )
                {
                    showInfoMessage( QMessageBox::tr("Commit completed"), QString(QMessageBox::tr("%1 out of %2 files were committed successfully")).arg(successfulIds.size()).arg(selected.size()) );
                }
                else
                {
                    showInfoMessage( QMessageBox::tr("Commit failed"), QMessageBox::tr("Files were not committed") );
                }
            }
            
            break;
        }
    }

    // Operation complete
    m_ProgressDialog->hide();
    delete m_ProgressDialog;
    m_ProgressDialog = NULL;
    
    // Update GUI
    updateGui();
}


//----------------------------------------------------------------------------
/*! 
 *  Commit canceled event handler
 */

void DPiqlIngestMainWindow::commitCanceled()
{
    ERROR_F( "DPiqlIngestMainWindow::commitCanceled" );

    setCommitCanceled( true );
}


//----------------------------------------------------------------------------
/*! 
 *  Delete file from list
 */

bool DPiqlIngestMainWindow::deleteFile( QString& errorMessage, int id )
{
    ERROR_F( "DPiqlIngestMainWindow::deleteFile" );

    if ( !m_FileList.removeFileById( id ) )
    {
        error << ERRerror << "Failed to delete file" << endl;
        errorMessage = QMessageBox::tr("Failed to delete file");
        return false;
    }
    
    error << ERRinfo << "Deleted file with id: " << id << endl;

    return true;
}


//----------------------------------------------------------------------------
/*! 
 *  Commit file from list
 */

bool DPiqlIngestMainWindow::commit(bool& canceled, QString& errorMessage, vector<int> ids, bool existenceMetadataPackage)
{
    ERROR_F( "DPiqlIngestMainWindow::commit" );

    const QString errorHeader = QMessageBox::tr("Failed to commit");

    // Validate id's
    if ( ids.size() == 0 )
    {
        errorMessage = "No files are selected for commit";
        error << ERRerror << "ID list is empty" << endl;
        return false;
    }

    // Get transaction id
    int transaction = m_Transaction;
    m_Transaction++;
    string transactionStr;
    if ( !DStringTools::toString(transaction, transactionStr) )
    {
        errorMessage = QMessageBox::tr("Failed to setup transaction");
        error << ERRerror << "Failed to convert transaction to string" << endl;
        return false;
    }

    // Save context
    if ( !saveContext() )
    {
        showErrorMessage( errorHeader, QMessageBox::tr("Failed to update context") );
        error << ERRerror << "Failed to update context" << endl;
        return false;
    }

    error << ERRinfo << "Starting commit: transaction=" << transaction << endl;

    // Create a temp directory for transaction
    DPath tempDir;
    if ( !createTempDir(tempDir, transaction) )
    {
        error << ERRerror << "Failed to create temp dir for transaction: " << transaction << endl;
        showErrorMessage(errorHeader, QMessageBox::tr("Failed to create directory for temporary files"));
        return false;
    }

    // Commit
    bool commitOk = false;

    string scriptFile = DPhpUtils::GetScriptPath( "commit-transaction.php" );
    if ( !scriptFile.length() )
    {
        errorMessage = QMessageBox::tr("Failed to create package");
        error << ERRerror << "File does not exist: " << scriptFile << endl;
        return false;
    }
    setCommitMessage( "" );
    setCommitStatus( DPiqlIngestMainWindow::COMMIT_STATUS_UNDEFINED );
    setCommitCanceled( false );
    if ( DPath( scriptFile ).extension() == "php" )
    {
        DPath contextPath;
        DScriptContext context;
        context.addParameterValue( "TempFileDirectory", tempDir.path() );
        for ( unsigned int i = 0; i < ids.size(); i++ )
        {
            // Get file
            DIngestFile ingestFile;
            if ( !m_FileList.getFileById(ingestFile, ids[i]) )
            {
                errorMessage = QMessageBox::tr("Failed to get file from list");
                error << ERRerror << errorMessage << endl;
                return false;
            }

            // Write file path
            context.addParameterValue( "FilePaths", ingestFile.fileName(true) );

            // Write relative file path
            string relativePath;
            if ( !m_FileList.getRelativePath(relativePath, ingestFile) )
            {
                errorMessage = QMessageBox::tr("Failed to get relative path of file");
                error << ERRerror << errorMessage << endl;
                return false;
            }
            context.addParameterValue( "RelativeFilePaths", relativePath );

            // Write item type
            context.addParameterValue( "FileItemTypes", ingestFile.typeAsString() );

            // Write file id
            string idStr;
            if ( !DStringTools::toString(ingestFile.id(), idStr) )
            {
                errorMessage = QMessageBox::tr("Invalid file id");
                error << ERRerror << errorMessage << endl;
                return false;
            }
            context.addParameterValue( "FileIds", idStr );

            // Write parent file id
            string parentIdStr;
            if ( !DStringTools::toString(ingestFile.parentId(), parentIdStr) )
            {
                errorMessage = QMessageBox::tr("Invalid file id");
                error << ERRerror << errorMessage << endl;
                return false;
            }
            context.addParameterValue( "ParentFileIds", parentIdStr );

            // Write metadata
            ostringstream metadata;
            if ( !ingestFile.metadataGroupList().write(metadata, false) )
            {
                errorMessage = QMessageBox::tr("Failed to write metadata");
                error << ERRerror << "Failed to write metadata to stream" << endl;
                return false;
            }
            context.addParameterValue( "FileMetadata", metadata.str() );
        }

        if (existenceMetadataPackage)
        {
            // Write package metadata
            ostringstream metadata;
            if (!m_PackageMetadata.write(metadata, false))
            {
                errorMessage = QMessageBox::tr("Failed to write package metadata");
                error << ERRerror << "Failed to write package metadata to stream" << endl;
                return false;
            }

            context.addParameterValue("PackageMetadata", metadata.str());
        }

        if ( !DPhpUtils::CreateScriptContext(contextPath, context, m_TempDir) )
        {
            error << ERRerror << "Failed to create context" << endl;
            return false;
        }
        else
        {
            // Execute script: script.php <context path>
            string command = phpPath() + " -f \"" + scriptFile + "\" \"" + m_Config->applicationLanguage() + "\" \"" + contextPath.path() + "\"";
            WorkerThread *workerThread = new WorkerThread(this, command);
            workerThread->start();

            int progress;
            string message;
            while ( !workerThread->wait(50) )
            {
                // Update GUI
                if ( workerThread->progress(progress, message) )
                {
                    m_ProgressDialog->setLabelText( QString::fromStdString(message) );
                    m_ProgressDialog->setValue(progress);
                }
                QCoreApplication::processEvents();
            }
            delete workerThread;

            if ( m_CommitStatus == COMMIT_STATUS_SUCCESS )
            {
                error << ERRinfo << "File was committed successfully" << endl;
                commitOk = true;
            }
            else if ( m_CommitCanceled )
            {
                error << ERRinfo << "Commit was canceled" << endl;
                canceled = true;
            }
            else if ( m_CommitStatus == COMMIT_STATUS_ERROR )
            {
                string scriptError = m_CommitMessage;
                errorMessage = QMessageBox::tr("Failed to create package");
                error << ERRerror << "Failed to execute command: " << command << endl;
                error << ERRerror << "Script error: " << scriptError << endl;
            }
            else
            {
                string scriptError = m_CommitMessage;
                errorMessage = QMessageBox::tr("Undefined error");
                error << ERRerror << "Undefined commit status: " << m_CommitStatus << endl;
                error << ERRerror << "Script error: " << scriptError << endl;
            }

            // Cleanup
            m_ProgressDialog->setLabelText( QProgressDialog::tr("Cleaning up") );
            if ( !commitOk )
            {
                bool cleanupOk = false;
                scriptFile = DPhpUtils::GetScriptPath( "cleanup-transaction.php" );
                if ( scriptFile.length() )
                {
                    DPath contextPath;
                    DScriptContext context;
                    context.addParameterValue( "TempFileDirectory", tempDir.path() );
                    if ( !DPhpUtils::CreateScriptContext(contextPath, context, m_TempDir) )
                    {
                        error << ERRerror << "Failed to create context" << endl;
                        return false;
                    }
                    else
                    {
                        command = phpPath() + " -f \"" + scriptFile + "\" \"" + m_Config->applicationLanguage() + "\" \"" + contextPath.path() + "\"";
                        string scriptErrorMessage;
                        if ( DSystemTools::doShellCommand(scriptErrorMessage, command) )
                        {
                            cleanupOk = true;
                        }
                        else
                        {
                            error << ERRerror << "Failed to execute command: " << command << endl;
                            error << ERRerror << "Script error message: " << scriptErrorMessage << endl;
                        }
                    }
                }
                else
                {
                    error << ERRerror << "Failed to run script: " << scriptFile << endl;
                }

                if ( !cleanupOk )
                {
                    showWarningMessage( QMessageBox::tr("Cleanup error"), QMessageBox::tr("Failed server cleanup") );
                }
            }
        }

        // Delete context
        if ( !DPath::deleteFile(contextPath) )
        {
            error << ERRerror << "Failed to delete context: " << contextPath.path();
        }
    }
    else
    {
        errorMessage = QMessageBox::tr("Failed to create package");
        error << ERRerror << "Create package script has invalid extension: " << DPath( scriptFile ).extension() << endl;
    }

    error << ERRinfo << "Finished committing transaction: " << transaction << endl;

    // Cleanup for transaction
    if ( !cleanup( transaction ) )
    {
        error << ERRerror << "Failed to clean temp dir for transaction: " << transaction << endl;
        showWarningMessage( errorHeader, QMessageBox::tr("Failed to delete temporary files") );
    }

    return commitOk;
}


//----------------------------------------------------------------------------
/*! 
 *  Edit metadata
 */

bool DPiqlIngestMainWindow::editMetadata( QString& errorMessage, vector<int> fileIds )
{
    ERROR_F( "DPiqlIngestMainWindow::editMetadata" );

    const QString errorMessageHeader = "Failed to edit metadata";

    // Remove duplicates
    removeDuplicates( fileIds );

    // Get files
    DIngestFileList ingestFiles;
    vector< int > metadataFileIds;
    for ( unsigned int i = 0; i < fileIds.size(); i++ )
    {
        DIngestFile ingestFile;
        if ( !m_FileList.getFileById(ingestFile, fileIds[i]) )
        {
            errorMessage = QMessageBox::tr("Failed to get file from list");
            error << ERRerror << errorMessage << endl;
            return false;
        }

        if ( ingestFile.type() == DIngestFile::TYPE_FILE )
        {
            ingestFiles.addFile( ingestFile );
            metadataFileIds.push_back( fileIds[i] );
        }
    }

    if ( ingestFiles.count() == 0 )
    {
        error << ERRinfo << "No files were selected" << endl;
        return true;
    }

    // Create dialog
    DEditMetadataWindowBase * editMetadataDialog = new DEditMetadataWindowFile( this, ingestFiles, phpPath(), m_TempDir, m_Config );
    
    // Ask user for metadata until it is valid or until
    // "Cancel" (or [X]) is pressed in the dialog
    if ( editMetadataDialog->hasError().length() )
    {
        showErrorMessage( QMessageBox::tr("Metadata dialog error"), QString(QMessageBox::tr("Failed to load metadata dialog (%1)")).arg( editMetadataDialog->hasError().c_str() ) );
        error << ERRerror << "Metadata dialog has errors" << endl;
        delete editMetadataDialog;
        return false;
    }
    while ( true )
    {
        if ( editMetadataDialog->exec() == QDialog::Accepted )
        {
            // Check if any metadata field is empty
            bool metadataValid = true;
            if ( !m_Config->allowEmptyMetadata() )
            {
                for ( unsigned int i = 0; i < ingestFiles.count() && metadataValid; i++ )
                {
                    DIngestFile ingestFile;
                    if ( !ingestFiles.getFileByIndex(ingestFile, i) )
                    {
                        errorMessage = QMessageBox::tr("Failed to get file from list");
                        return false;
                    }

                    for (unsigned int j = 0; j < ingestFile.metadataGroupList().groupCount() && metadataValid; j++)
                    {
                        DMetadataItemGroup group;
                        if ( !ingestFile.metadataGroupList().getGroup(group, j) )
                        {
                            errorMessage = QMessageBox::tr("Failed to get group from list");
                            return false;
                        }

                        for (unsigned int k = 0; k < group.itemCount() && metadataValid; k++)
                        {
                            DMetadataItem metadataItem;
                            if ( !group.getItem(metadataItem, k) )
                            {
                                errorMessage = QMessageBox::tr("Failed to get metadata from list");
                                return false;
                            }
                            if ( metadataItem.value().empty() )
                            {
                                showErrorMessage(QMessageBox::tr("Empty metadata"), QMessageBox::tr("All metadata fields need to be filled in"));
                                error << ERRerror << "Some metadata fields are empty" << endl;
                                metadataValid = false;
                                break;
                            }
                        }
                    }
                }
            }

            // Validate
            if ( !metadataValid )
            {
                // No metadata should be saved
            }
            else if ( editMetadataDialog->validate() )
            {
                if ( m_FileList.setFilesById(ingestFiles, metadataFileIds) )
                {
                    // Save context
                    if ( !saveContext() )
                    {
                        showErrorMessage(QMessageBox::tr("Error"), QMessageBox::tr("Failed to update context"));
                        error << ERRerror << "Failed to update context" << endl;
                    }

                    // Metadata is set
                    if ( updateInformationViewMetadata(metadataFileIds[metadataFileIds.size() - 1]) )
                    {
                        break;
                    }
                    else
                    {
                        showWarningMessage(errorMessageHeader, QMessageBox::tr("Failed to show metadata"));
                        error << ERRwarning << "Failed to show metadata" << endl;
                        break;
                    }
                }
                else
                {
                    showErrorMessage(errorMessageHeader, QMessageBox::tr("Failed to store file info"));
                    error << ERRerror << "Failed to set file info" << endl;
                }
            }
            else
            {
                if ( editMetadataDialog->hasError().size() )
                {
                    showErrorMessage( errorMessageHeader, editMetadataDialog->hasError().c_str() );
                    error << ERRerror << "Edit dialog error: " << editMetadataDialog->hasError() << endl;
                }
                else
                {
                    showErrorMessage( errorMessageHeader, QMessageBox::tr("Metadata is invalid") );
                    error << ERRerror << "Failed to validate metadata" << endl;
                }
                editMetadataDialog->clearError();
            }
        }
        else
        {
            // Dialog was closed - disregard metadata
            error << ERRinfo << "Dialog was closed without saving" << endl;
            break;
        }
    }

    delete editMetadataDialog;
    return true;
}


bool DPiqlIngestMainWindow::createEditMetadataDialog(QString& errorMessage, DEditMetadataWindowBase * editMetadataDialog, DMetadataItemGroupList& metadataGroupList)
{
    ERROR_F("DPiqlIngestMainWindow::createEditMetadataDialog");

    // Ask user for metadata until it is valid or until
    // "Cancel" (or [X]) is pressed in the dialog
    if (editMetadataDialog->hasError().length())
    {
        showErrorMessage(QMessageBox::tr("Metadata dialog error"), QString(QMessageBox::tr("Failed to load metadata dialog (%1)")).arg(editMetadataDialog->hasError().c_str()));
        error << ERRerror << "Metadata dialog has errors" << endl;
        delete editMetadataDialog;
        return false;
    }
    while (true)
    {
        if (editMetadataDialog->exec() == QDialog::Accepted)
        {
            // Check if any metadata field is empty
            bool metadataValid = true;
            if (!m_Config->allowEmptyMetadata())
            {
                for (unsigned int j = 0; j < metadataGroupList.groupCount() && metadataValid; j++)
                {
                    DMetadataItemGroup group;
                    if (!metadataGroupList.getGroup(group, j))
                    {
                        errorMessage = QMessageBox::tr("Failed to get group from list");
                        return false;
                    }

                    for (unsigned int k = 0; k < group.itemCount() && metadataValid; k++)
                    {
                        DMetadataItem metadataItem;
                        if (!group.getItem(metadataItem, k))
                        {
                            errorMessage = QMessageBox::tr("Failed to get metadata from list");
                            return false;
                        }
                        if (metadataItem.value().empty())
                        {
                            showErrorMessage(QMessageBox::tr("Empty metadata"), QMessageBox::tr("All metadata fields need to be filled in"));
                            error << ERRerror << "Some metadata fields are empty" << endl;
                            metadataValid = false;
                            break;
                        }
                    }
                }
            }

            // Validate
            if (!metadataValid)
            {
                // No metadata should be saved
            }
            else if (editMetadataDialog->validate())
            {
                editMetadataDialog->clearError();
                break;
            }
        }
        else
        {
            // Dialog was closed - disregard metadata
            errorMessage = QMessageBox::tr("Package metadata editing has been canceled");
            error << ERRinfo << "Dialog was closed without saving" << endl;
            
            return false;
        }
    }

    return true;
}


//----------------------------------------------------------------------------
/*! 
 *  Add files to structure
 */

bool DPiqlIngestMainWindow::addFiles( const QStringList& files )
{
    ERROR_F( "DPiqlIngestMainWindow::addFiles" );

    const QString errorHeader = "Add files error";

    if ( files.size() == 0 )
    {
        error << ERRinfo << "There are no files to add" << endl;
        return false;
    }

    // Use OS specific directory separators
    QStringList fileNames = files;
    for ( int i = 0; i < fileNames.size(); i++ )
    {
        if ( QFileInfo(fileNames[i]).isDir() )
        {
            fileNames[i] += "/";
        }
        fileNames[i] = QDir::toNativeSeparators( fileNames[i] );
    }

    // Check for duplicates
    const bool allowDuplicates = false;
    if ( !allowDuplicates )
    {
        for ( int i = 0; i < fileNames.count(); i++ )
        {
            for ( unsigned int j = 0; j < m_FileList.count(); j++ )
            {
                DIngestFile file;
                if ( !m_FileList.getFileByIndex( file, j ) )
                {
                    error << ERRerror << "Failed to get file from list" << endl;
                    showErrorMessage( errorHeader, QMessageBox::tr("Failed to get file from list") );
                    return false;
                }

                if ( fileNames[i].toStdString() == file.fileName( true ) )
                {
                    error << ERRerror << "Same file was added twice: " << fileNames[i] << endl;
                    showErrorMessage( errorHeader, QMessageBox::tr("The file was already added: ") + file.fileName( false ).c_str() );
                    return false;
                }
            }
        }
    }

    // Add files
    DIngestFileList fileListBackup = m_FileList;
    vector<int> addedIds;
    for ( int i = 0; i < fileNames.count(); i++ )
    {
        QString fileName = fileNames[i];

        if ( !fileName.isEmpty() )
        {
            if ( !m_FileList.addFileFromDisk(addedIds, fileName.toStdString()) )
            {
                // Restore list
                m_FileList = fileListBackup;

                // Update GUI
                updateGui();

                error << ERRerror << "File is invalid: " << fileName << endl;
                showErrorMessage( errorHeader, QMessageBox::tr("Failed to add file: ") + fileName.toStdString().c_str() );
            }
        }
    }

    // Don't allow spaces in filenames
    string regex = m_Config->validateFilenameRegex();
    for ( unsigned int i = 0; i < addedIds.size(); i++ )
    {
        DIngestFile newFile;
        if ( !m_FileList.getFileById(newFile, addedIds[i]) )
        {
            // Restore list
            m_FileList = fileListBackup;
            
            error << ERRerror << "File with id " << addedIds[i] << " could not be fetched from list" << endl;
            showErrorMessage( errorHeader, QMessageBox::tr("Failed to fetch file from list") );
            return false;
        }

        string compareFileName;
        DPath path( newFile.fileName(true) );
        if ( path.filename().size() )
        {
            compareFileName = path.filename();
        }
        else if ( path.pathList().size() >= 2 )
        {
            compareFileName = path.pathList()[path.pathList().size()-2];
        }
        if ( !compareFileName.size() || !DRegExpUtil::Match(compareFileName, regex) )
        {
            // Restore list
            m_FileList = fileListBackup;

            error << ERRerror << "File '" << compareFileName << "' does not match regex: '" << regex << "'" << endl;
            showErrorMessage( errorHeader, QString("File '%1' contains invalid characters").arg(compareFileName.c_str()) );
            return false;
        }
    }

    // Save context
    if ( !saveContext() )
    {
        showErrorMessage(QMessageBox::tr("Error"), QMessageBox::tr("Failed to update context"));
        error << ERRerror << "Failed to update context" << endl;
    }

    // Update GUI
    updateGui();

    // Open dialog for editing metadata
    if ( m_Config->autoOpenMetadataEdit() )
    {
        QString errorMessage;
        if ( !editMetadata(errorMessage, addedIds) )
        {
            // Failed to add metadata
            error << ERRerror << "Failed to edit metadata" << endl;
            showErrorMessage( QMessageBox::tr("Edit metadata failed"), errorMessage );
            return false;
        }
    }

    // Update GUI
    updateGui();

    error << ERRinfo << "Files were added successfully" << endl;

    return true;
}


//----------------------------------------------------------------------------
/*! 
 *  Update file tree
 */

void DPiqlIngestMainWindow::updateGui()
{
     ERROR_F( "DPiqlIngestMainWindow::updateGui" );

    // Populate tree
    populateTree();

    // Clear information view
    m_Ui.informationView->clear();

    // Disable buttons
    m_Ui.editMetadataButton->setDisabled( true );
    m_Ui.deleteFilesButton->setDisabled( true );
    m_Ui.commitButton->setDisabled( true );
}


//----------------------------------------------------------------------------
/*!
 *  Update file tree
 */

bool DPiqlIngestMainWindow::populateTree()
{
    ERROR_F( "DPiqlIngestMainWindow::populateTree" );

    // Clear tree
    m_Ui.treeWidget->clear();
    m_FileTreeMap.clear();

    if ( m_FileList.count() > 0 )
    {
        for ( unsigned int i = 0; i < m_FileList.count(); i++ )
        {
            DIngestFile file;
            if ( !m_FileList.getFileByIndex(file, i) )
            {
                error << ERRerror << "Failed to get file from list" << endl;
                showErrorMessage(QMessageBox::tr("Failed to update list"), QMessageBox::tr("Failed to update file list"));
                return false;
            }

            if ( !file.hasParent() )
            {
                populateTreeAddNode( m_Ui.treeWidget, file );
            }
        }

        // Enable tree view
        m_Ui.treeWidget->setDisabled( false );
    }

    return true;
}


//----------------------------------------------------------------------------
/*!
 *  Update file tree
 */

bool DPiqlIngestMainWindow::populateTreeAddNode( QTreeWidget* tree, DIngestFile file, QTreeWidgetItem* parent /*= NULL*/ )
{
    ERROR_F( "DPiqlIngestMainWindow::populateTreeAddNode" );

    // Create node
    QTreeWidgetItem* node = new QTreeWidgetItem;
    m_FileTreeMap.add( node, file.id() );
    if ( file.type() == DIngestFile::TYPE_DIRECTORY )
    {
        std::vector<std::string> path = DPath( file.fileName(true) ).pathList();
        if ( path.size() >= 2 )
        {
            node->setText( 0, QString::fromStdString(path[path.size() - 2]) );
        }
        else
        {
            error << ERRerror << "Directory name is blank" << endl;
            showErrorMessage( QMessageBox::tr("File path error"), QMessageBox::tr("Directory name is blank") );
            return false;
        }

        // Add node to tree
        if ( parent == NULL )
        {
            m_Ui.treeWidget->addTopLevelItem( node );
        }
        else
        {
            parent->addChild( node );
        }
    }
    else if ( file.type() == DIngestFile::TYPE_FILE )
    {
        node->setText( 0, QString::fromStdString( file.fileName(false) ) );

        // Add node to tree
        if ( parent == NULL )
        {
            m_Ui.treeWidget->addTopLevelItem( node );
        }
        else
        {
            parent->addChild( node );
        }
    }
    else
    {
        error << ERRerror << "File has unknown type: " << file.type() << endl;
        showErrorMessage(QMessageBox::tr("Unknown type"), QMessageBox::tr("File has unknown type"));
        return false;
    }

    for ( unsigned int j = 0; j < m_FileList.count(); j++ )
    {
        DIngestFile compareFile;
        if ( !m_FileList.getFileByIndex(compareFile, j) )
        {
            error << ERRerror << "Failed to get file from list" << endl;
            showErrorMessage(QMessageBox::tr("Failed to update list"), QMessageBox::tr("Failed to update file list"));
            return false;
        }

        if ( compareFile.parentId() == file.id() )
        {
            if ( !populateTreeAddNode(tree, compareFile, node) )
            {
                return false;
            }
        }
    }

    return true;
}


//----------------------------------------------------------------------------
/*!
 *  Check for duplicates in file list
 */

bool DPiqlIngestMainWindow::hasDuplicates( const vector< int >& selectedIds ) const
{
    ERROR_F( "DPiqlIngestMainWindow::hasDuplicates" );

    vector< int > sorted = selectedIds;
    std::sort( sorted.begin(), sorted.end() );
    for ( unsigned int i = 0; i < sorted.size(); i++ )
    {
        if ( i > 0 && sorted[i] == sorted[i - 1] )
        {
            return true;
        }
    }

    return false;
}


//----------------------------------------------------------------------------
/*!
 *  Remove duplicates from file list
 */

void DPiqlIngestMainWindow::removeDuplicates( vector< int >& selectedIds ) const
{
    if ( hasDuplicates(selectedIds) )
    {
        for ( unsigned int i = 0; i < selectedIds.size(); i++ )
        {
            for ( unsigned int j = i + 1; j < selectedIds.size(); j++ )
            {
                if ( selectedIds[i] == selectedIds[j] )
                {
                    selectedIds.erase( selectedIds.begin() + j );
                    j--;
                }
            }
        }
    }
}


//----------------------------------------------------------------------------
/*! 
 *  Handle activated tree item
 */

void DPiqlIngestMainWindow::activatedTreeItem()
{
    if ( m_Ui.treeWidget->selectedItems().size() == 0 )
    {
        activatedTreeItem( NULL, 0 );
    }
    else
    {
        activatedTreeItem( m_Ui.treeWidget->currentItem(), 0 );
    }
}


//----------------------------------------------------------------------------
/*! 
 *  Handle activated tree item
 */

void DPiqlIngestMainWindow::activatedTreeItem( QTreeWidgetItem* item, int /*column*/ )
{
    ERROR_F( "DPiqlIngestMainWindow::activatedTreeItem" );

    int id;
    if ( item == NULL || !m_FileTreeMap.getId(id, item) || !m_FileList.idExists(id) )
    {
        // Item was not found
        // Deactivate buttons
        m_Ui.editMetadataButton->setDisabled( true );
        m_Ui.deleteFilesButton->setDisabled( true );
        m_Ui.commitButton->setDisabled( true );
        return;
    }

    // Activate buttons
    m_Ui.editMetadataButton->setDisabled( false );
    m_Ui.deleteFilesButton->setDisabled( false );
    m_Ui.commitButton->setDisabled( false );

    // Update information view
    if ( !updateInformationViewMetadata( id ) )
    {
        error << ERRerror << "Failed to update metadata in GUI" << endl;
    }

    // Activate information view
    m_Ui.informationView->setEnabled( true );
}

bool DPiqlIngestMainWindow::updateInformationViewMetadata( int itemId )
{
    ERROR_F( "DPiqlIngestMainWindow::updateInformationViewMetadata" );

    // Clear information view
    m_Ui.informationView->clear();

    // Get file
    DIngestFile ingestFile;
    if ( !m_FileList.getFileById(ingestFile, itemId) )
    {
        error << ERRerror << "Failed to get file from list" << endl;
        return false;
    }

    // Get metadata
    QString metadata;
    const DMetadataItemGroupList& metadataGroupList = ingestFile.metadataGroupList();
    for ( unsigned int i = 0; i < metadataGroupList.groupCount(); i++ )
    {
        DMetadataItemGroup group;
        if ( !metadataGroupList.getGroup(group, i) )
        {
            error << ERRerror << "Failed to get group" << endl;
            return false;
        }

        QString metadataGroupName = QString("%1\n").arg(QString::fromStdString(group.name()));
        if (metadataGroupName.length() > 0)
        {
            metadataGroupName[0] = metadataGroupName[0].toUpper();
        }
        metadata += metadataGroupName;

        for ( unsigned int j = 0; j < group.itemCount(); j++ )
        {
            DMetadataItem metadataItem;
            if ( !group.getItem(metadataItem, j) )
            {
                error << ERRerror << "Failed to get metadata item" << endl;
                return false;
            }

            QString itemKey = QString::fromStdString(metadataItem.key());
            if (itemKey.length() > 0)
            {
                itemKey[0] = itemKey[0].toUpper();
            }
            QString itemValue = QString::fromStdString(metadataItem.value());

            metadata += QString("  %1: %2\n").arg(itemKey).arg(itemValue);
        }
    }

    // Show info
    m_Ui.informationView->setPlainText( metadata );
    m_Ui.informationView->setEnabled( true );

    return true;
}

string DPiqlIngestMainWindow::phpPath() const
{
#if defined (D_OS_X)

    QDir dir = QDir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    
    // Run from resources folder
    return "cd " + dir.path().toStdString() + "/Contents/Resources ; php";
#elif defined ( D_OS_UNIX )
    return "php";
#elif defined ( D_OS_WIN32 )
    return "php\\php.exe";
#endif
}

bool DPiqlIngestMainWindow::createTempDir( DPath& path, int transaction )
{
    ERROR_F( "DPiqlIngestMainWindow::createTempDir" );

    // Check if transaction is valid
    if ( transaction < 0 || transaction > m_Transaction )
    {
        return false;
    }

    // Cleanup data from former transaction
    if ( !cleanup( transaction ) )
    {
        error << ERRerror << "Failed to cleanup transaction " << transaction << endl;
        return false;
    }

    // Get path of transaction dir
    DPath transactionDir( m_TempDir );
    string transactionStr;
    if ( !DStringTools::toString( transaction, transactionStr ) )
    {
        error << ERRerror << "Failed to convert transaction number to int" << endl;
        return false;
    }
    string newDir = "transaction_" + transactionStr;
    if ( !transactionDir.appendDir( newDir ) )
    {
        error << ERRerror << "Failed to append dir '" + newDir + "' to path '" + transactionDir.path() + "'";
        return false;
    }

    // Create directory
    if ( !DPath::makeDir( transactionDir ) )
    {
        error << ERRerror << "Failed to create transaction dir: " << endl;
        return false;
    }

    // Return path
    path = transactionDir;

    return true;
}

bool DPiqlIngestMainWindow::cleanup( int transaction /*= -1*/ )
{
    ERROR_F( "DPiqlIngestMainWindow::cleanup" );

    // Create temp dir if it doesn't exists
    if ( !DPath::isExistingDir( m_TempDir ) )
    {
        const bool recursive = true;
        if ( !DPath::makeDir( m_TempDir, recursive ) )
        {
            error << ERRerror << "Failed to create temp directory: " << m_TempDir.path() << endl;
            return false;
        }
    }

    // Create data dir if it doesn't exists
    if ( !DPath::isExistingDir(m_DataDir) )
    {
        const bool recursive = true;
        if ( !DPath::makeDir(m_DataDir, recursive) )
        {
            error << ERRerror << "Failed to create data directory: " << m_DataDir.path() << endl;
            return false;
        }
    }

    // Create reports dir if it doesn't exists
    if ( !DPath::isExistingDir(m_ReportsDir) )
    {
        const bool recursive = true;
        if ( !DPath::makeDir(m_ReportsDir, recursive) )
        {
            error << ERRerror << "Failed to create reports directory: " << m_ReportsDir.path() << endl;
            return false;
        }
    }

    // Clean temp dir
    if ( !m_Config->deleteTempFiles() )
    {
        error << ERRinfo << "Cleaning of temp files is disabled" << endl;
    }
    else if ( transaction >= 0 )
    {
        // Get path of transaction dir
        DPath transactionDir( m_TempDir );
        string transactionStr;
        if ( !DStringTools::toString( transaction, transactionStr ) )
        {
            error << ERRerror << "Failed to convert transaction number to int" << endl;
            return false;
        }
        string newDir = "transaction_" + transactionStr;
        if ( !transactionDir.appendDir( newDir ) )
        {
            error << ERRerror << "Failed to append dir '" + newDir + "' to path '" + transactionDir.path() + "'";
            return false;
        }

        // Check if dir exists
        if ( !DPath::isExistingDir( transactionDir ) )
        {
            return true;
        }

        // Delete dir and all files in it
        const bool removeFiles = true;
        if ( !DPath::deleteDir( transactionDir, removeFiles ) )
        {
            error << ERRerror << "Failed to delete transaction temp dir: " << transactionDir.path() << endl;
            return false;
        }
    }
    else
    {
        if ( !DPath::cleanDir( m_TempDir ) )
        {
            error << ERRerror << "Failed to clean temp dir: " << m_TempDir.path() << endl;
            return false;
        }
    }

    return true;
}

void DPiqlIngestMainWindow::setCommitMessage( const std::string& message )
{
    ERROR_F( "DPiqlIngestMainWindow::setCommitMessage" );

    m_CommitMessage = message;
}

void DPiqlIngestMainWindow::setCommitStatus( int status )
{
    ERROR_F( "DPiqlIngestMainWindow::setCommitStatus" );

    m_CommitStatus = status;
}

void DPiqlIngestMainWindow::setCommitCanceled( bool state )
{
    ERROR_F( "DPiqlIngestMainWindow::setCommitCanceled" );

    m_CommitCanceled = state;
}

bool DPiqlIngestMainWindow::saveContext() const
{
    ERROR_F("DPiqlIngestMainWindow::saveContext");

    // Compose XML
    stringstream xml;
    xml <<
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<context>"
        "<version>1.0.1</version>"
        "<transaction>" << m_Transaction << "</transaction>";

    if ( !m_FileList.write( xml, false ) )
    {
        error << ERRerror << "Failed to write file list" << endl;
        return false;
    }

    xml << "</context>" << endl;

    // Write to file
    DPath outFilePath( m_DataDir.directory(), "context.xml" );
    ofstream outFile( outFilePath.path().c_str(), std::iostream::binary | std::iostream::out );
    outFile << xml.str();
    if ( !outFile.good() )
    {
        error << ERRerror << "Failed to write context to " << outFilePath.path() << endl;
        return false;
    }

    return true;
}

bool DPiqlIngestMainWindow::loadContext()
{
    ERROR_F("DPiqlIngestMainWindow::loadContext");

    DPath filePath( m_DataDir.directory(), "context.xml" );

    // Create context if it doesn't exist
    if ( !DFileSystemTools::isExistingPath(filePath.path()) )
    {
        if ( !saveContext() )
        {
            error << ERRerror << "Failed to create default context: " << filePath.path() << endl;
            return false;
        }
    }

    // Open file
    QFile file( QString::fromLocal8Bit(filePath.path().c_str()) );
    if ( !file.open(QIODevice::ReadOnly) )
    {
        error << ERRerror << "Failed to open: " << filePath.path() << endl;
        return false;
    }

    // Load XML
    QDomDocument doc("context");
    QString errorMessage;
    int errorLine;
    int errorColumn;
    if ( !doc.setContent(&file, &errorMessage, &errorLine, &errorColumn) )
    {
        error << ERRerror
            << "Failed to set document content. "
            << "File=" << filePath.path()
            << " Error='" << errorMessage.toStdString()
            << "' at line=" << errorLine
            << " column=" << errorColumn
            << endl;

        return false;
    }
    file.close();

    // Get the root element
    QDomElement docElem = doc.documentElement();

    // Check root tag name
    QString rootTag = docElem.tagName();
    if ( rootTag != "context" )
    {
        error << ERRerror << "context is not root element but: " << rootTag.toStdString() << endl;
        return false;
    }

    // If version is missing - context is ignored
    if ( docElem.elementsByTagName("version").size() == 0 )
    {
        error << ERRinfo << "Version is missing" << endl;
        return true;
    }
    else
    {
        string version = docElem.elementsByTagName("version").at(0).toElement().text().toStdString();
        error << ERRinfo << "Version is " << version << endl;
        if ( version == "1.0.0" )
        {
            // Add element <group>
            QFile inFile( QString::fromStdString(filePath.path()) );
            if ( !inFile.open(QIODevice::ReadOnly | QIODevice::Text) )
            {
                error << ERRerror << "Failed to read context: " << filePath.path() << endl;
                return false;
            }
            QString newXml;
            QTextStream in(&inFile);
            while ( !in.atEnd() )
            {
                QString line = in.readLine();
                line.replace( "<metadata>", "<metadata><group name=\"undefined\">" );
                line.replace("</metadata>", "</metadata></group>");
                newXml += line;
            }
            inFile.close();

            QFile outFile( QString::fromStdString(filePath.path()) );
            if ( !inFile.open(QIODevice::WriteOnly | QIODevice::Text) )
            {
                error << ERRerror << "Failed to write context: " << filePath.path() << endl;
                return false;
            }
            QTextStream out(&outFile);
            out << newXml;
            outFile.close();
        }
        else if (version == "1.0.1")
        {
            // Do nothing
        }
        else
        {
            error << ERRerror << "Invalid context version: " << version << endl;
            return false;
        }
    }

    // Read files
    QDomNodeList fileList = docElem.elementsByTagName("files");
    if ( fileList.size() == 1 )
    {
        QDomElement filesElement = fileList.at(0).toElement();
        if ( !DIngestFileList::Read(m_FileList, filesElement) )
        {
            error << ERRerror << "failed to read files section" << endl;
            return false;
        }
    }
    else if ( fileList.size() > 1 )
    {
        error << ERRerror << "more than one files tag found" << endl;
        return false;
    }
    else
    {
        error << ERRinfo << "No files tag found" << endl;
        return false;
    }

    // Read transaction
    QDomNodeList transactions = docElem.elementsByTagName("transaction");
    if ( transactions.size() == 1 )
    {
        QDomElement transactionElement = transactions.at(0).toElement();
        m_Transaction = transactionElement.text().toInt();

        if ( m_Transaction < 0 )
        {
            error << ERRerror << "Invalid transaction number: " << m_Transaction << endl;
            return false;
        }
    }
    else if ( transactions.size() > 1 )
    {
        error << ERRerror << "more than one transaction tag found" << endl;
        return false;
    }
    else
    {
        error << ERRinfo << "No transaction tag found" << endl;
        return false;
    }

    return true;
}

bool DPiqlIngestMainWindow::writeTransactionReport( int transactionId, vector<int>& fileIds, bool commitOk, bool canceled )
{
    ERROR_F( "DPiqlIngestMainWindow::writeTransactionReport" );

    string transactionStr;
    if ( !DStringTools::toString(transactionId, transactionStr) )
    {
        error << ERRerror << "Failed to convert transaction to string" << endl;
        return false;
    }

    // Commit status
    string commitStatus = "FAILURE";
    if ( commitOk )
    {
        commitStatus = "OK";
    }

    // Cancel status
    string canceledStatus = "FALSE";
    if ( canceled )
    {
        canceledStatus = "TRUE";
    }

    // Build file list
    DIngestFileList files;
    for ( unsigned int i = 0; i < fileIds.size(); i++ )
    {
        // Get file
        DIngestFile file;
        if ( !m_FileList.getFileById(file, fileIds[i]) )
        {
            error << ERRerror << "Failed to get file from list" << endl;
            return false;
        }

        files.addFile( file );
    }

    // Build xml
    stringstream xml;
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << endl;
    xml << "<transaction id=\"" << transactionId << "\">" << endl;
    xml << " <wasCanceled>" << canceledStatus << "<wasCanceled>" << endl;
    xml << " <commitStatus>" << commitStatus << "<commitStatus>" << endl;
    if ( !files.write(xml, false) )
    {
        error << ERRerror << "Failed to write file list" << endl;
        return false;
    }
    xml << "</transaction>" << endl;

    // Write to file
    string reportFilename = "transaction_report_" + transactionStr + ".xml";
    DPath outFilePath( m_ReportsDir.directory(), reportFilename );
    ofstream outFile( outFilePath.path().c_str(), std::iostream::binary | std::iostream::out );
    outFile << xml.str();
    if ( !outFile.good() )
    {
        error << ERRerror << "Failed to write context to " << outFilePath.path() << endl;
        return false;
    }

    error << ERRinfo << "Wrote report to " << outFilePath.path() << endl;

    return true;
}

/****************************************************************************/
/*! \class WorkerThread dpiqlingestmainwindow.h
 *  \ingroup PiqlIngest
 *  \brief Worker for executing shell commands
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

WorkerThread::WorkerThread( DPiqlIngestMainWindow* mainDialog, const std::string& command ) 
  : m_MainDialog( mainDialog ),
    m_HasProgress( false ),
    m_Command( command )
{
}

bool WorkerThread::progress( int& progress, string& message )
{
    if ( !m_HasProgress )
    {
        return false;
    }

    m_Mutex.lock();
    progress = m_Progress;
    message = m_Message;
    m_HasProgress = false;
    m_Mutex.unlock();

    return true;
}

//===================================
//  P R I V A T E   I N T E R F A C E
//===================================

void WorkerThread::run()
{
    ERROR_F( "WorkerThread::run" );

    error << ERRinfo << "Executing command: " << m_Command << endl;
    
    if ( !DSystemTools::doShellCommand( updateProgress, cancelExecution, m_Command, this ) )
    {
        error << ERRerror << "Failed to execute command: " << m_Command << endl;
    }
}

bool WorkerThread::updateProgress( std::string& progress, void* data )
{
    ERROR_F( "DPiqlIngestMainWindow::updateProgress" );
    
    WorkerThread* workerThread = (WorkerThread*)data;
    DPiqlIngestMainWindow* mainDialog = (DPiqlIngestMainWindow*)(workerThread->m_MainDialog);
    DPiqlIngestProgressDialog* progressDialog = mainDialog->m_ProgressDialog;

    if ( !progressDialog )
    {
        return false;
    }
    
    error << ERRdebug << progress << endl;
    
    // Get type
    vector<string> words;
    if ( !DStringTools::stringSplit( progress, words, " " ) || words.size() < 1 )
    {
        // Ignore output with no type
        return true;
    }
    string type = words[0];
    
    if ( type == "SUCCESS" || mainDialog->m_CommitStatus == DPiqlIngestMainWindow::COMMIT_STATUS_SUCCESS )
    {
        mainDialog->setCommitStatus( DPiqlIngestMainWindow::COMMIT_STATUS_SUCCESS );
    }
    else if ( type == "PROGRESS" )
    {
        // Get percentage
        if ( words.size() < 2 )
        {
            error << ERRerror << "Failed to get percentage from progress: " << progress << endl;
            mainDialog->setCommitStatus( DPiqlIngestMainWindow::COMMIT_STATUS_ERROR );
            return false;
        }
        int percentage;
        if ( !DStringTools::toInt( words[1], percentage ) )
        {
            error << ERRerror << "Failed to convert percentage: " << progress << endl;
            mainDialog->setCommitStatus( DPiqlIngestMainWindow::COMMIT_STATUS_ERROR );
            return false;
        }

        // Get status text
        string status;
        for ( unsigned int i = 2; i < words.size(); i++ )
        {
            status += words[i] + " ";
            if ( i == words.size() - 1 )
            {
                status = status.substr( 0, status.size() - 1 );
            }
        }

        workerThread->m_Mutex.lock();
        workerThread->m_Progress = percentage;
        workerThread->m_Message = status;
        workerThread->m_HasProgress = true;
        workerThread->m_Mutex.unlock();
    }
    else if ( type == "ERROR" )
    {
        // Get status text
        string errorMessage;
        for ( unsigned int i = 1; i < words.size(); i++ )
        {
            errorMessage += words[i] + " ";
            if ( i == words.size() - 1 )
            {
                errorMessage = errorMessage.substr( 0, errorMessage.size() - 1 );
            }
        }

        mainDialog->setCommitMessage( errorMessage );
        mainDialog->setCommitStatus( DPiqlIngestMainWindow::COMMIT_STATUS_ERROR );
    }
    else
    {
        error << ERRwarning << "Wrong output type, ignoring: " << progress << endl;
        return true;
    }

    return true;
}

bool WorkerThread::cancelExecution( void* data )
{
    ERROR_F( "DPiqlIngestMainWindow::cancelExecution" );

    WorkerThread* workerThread = (WorkerThread*)data;
    DPiqlIngestMainWindow* mainDialog = (DPiqlIngestMainWindow*)(workerThread->m_MainDialog);

    if ( mainDialog->m_CommitCanceled && mainDialog->m_CommitStatus != DPiqlIngestMainWindow::COMMIT_STATUS_SUCCESS )
    {
        mainDialog->m_ProgressDialog->setLabelText(QProgressDialog::tr("Cancelling commit"));
        return true;
    }

    return false;
}
