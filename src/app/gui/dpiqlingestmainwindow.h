#ifndef DPIQLINGESTMAINWINDOW_H
#define DPIQLINGESTMAINWINDOW_H

/*****************************************************************************
**  
**  Definition of the DPiqlIngestMainWindow class
**
**  Creation date:  2017/03/29
**  Created by:     Matti Wennberg
**
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the ingest tool application
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "ui_dpiqlingestmainwindow.h"
#include    "dingestfilelist.h"
#include    "dpiqlingestprogressdialog.h"
#include    "piql_ingest_config.h"

//  BASE INCLUDES
//
#include    "dcommon.h"
#include    "dpath.h"

//  QT INCLUDES
//
#include    <QThread>
#include    <QMutex>
#include    <QTreeWidget>

//  FORWARD DECLARATIONS
//
class QProgressDialog;

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )


//  FORWARD DECLARATIONS
//
class DEditMetadataWindowBase;

//============================================================================
// CLASS: FileTreeMap

class FileTreeMap
{
public:
    void add( QTreeWidgetItem* node, int id )
    {
        m_Nodes.push_back( node );
        m_Ids.push_back( id );
    };
    void clear()
    {
        m_Nodes.clear();
        m_Ids.clear();
    };
    bool getId( int& id, QTreeWidgetItem* node ) const
    {
        id = -1;
        if ( m_Nodes.size() != m_Ids.size() )
        {
            return false;
        }
        for ( unsigned int i = 0; i < m_Nodes.size(); i++ )
        {
            if ( m_Nodes[i] == node )
            {
                id = m_Ids[i];
                return true;
            }
        }
        return false;
    };
    QTreeWidgetItem* getNode( int id )
    {
        for ( unsigned int i = 0; i < m_Ids.size(); i++ )
        {
            if ( m_Ids[i] == id )
            {
                return m_Nodes[i];
            }
        }
        return NULL;
    }
    std::vector<int> getExpandedIds() const
    {
        std::vector<int> ids;
        for (unsigned int i = 0; i < m_Nodes.size(); i++)
        {
            if ( m_Nodes[i]->isExpanded() )
            {
                ids.push_back( m_Ids[i] );
            }
        }
        return ids;
    }

private:
    std::vector<QTreeWidgetItem*> m_Nodes;
    std::vector<int>              m_Ids;
};

//============================================================================
// CLASS: DPiqlIngestMainWindow
class DPiqlIngestMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum CommitStatus
    {
        COMMIT_STATUS_UNDEFINED,
        COMMIT_STATUS_ERROR,
        COMMIT_STATUS_SUCCESS
    };

public:
    DPiqlIngestMainWindow( DPiqlIngestConfig* config );
    ~DPiqlIngestMainWindow();

    void setCommitMessage( const std::string& message );
    void setCommitStatus( int status );

private slots:
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dropEvent( QDropEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    void selectFilesButtonPressed();
    void editMetadataButtonPressed();
    void deleteFilesButtonPressed();
    void commitButtonPressed();
    void activatedTreeItem( QTreeWidgetItem* item, int column );
    void activatedTreeItem();
    void commitCanceled();

private:
    void              showErrorMessage( const QString& header, const QString& message );
    void              showWarningMessage( const QString& header, const QString& message );
    void              showInfoMessage( const QString& header, const QString& message );
    std:: vector<int> selectedIds(bool recursive = false, int fileId = -1) const;
    bool              editMetadata( QString& errorMessage, std::vector<int> fileIds );
    bool              addFiles( const QStringList& files );
    bool              deleteFile( QString& errorMessage, int id );
    bool              commit(bool& canceled, QString& errorMessage, std::vector<int> ids, bool existenceMetadataPackage);
    void              updateGui();
    bool              updateInformationViewMetadata( int itemId );
    std::string       phpPath() const;
    bool              createTempDir( DPath& path, int transaction );
    bool              cleanup( int transaction = -1 );
    void              setCommitCanceled( bool state );
    bool              saveContext() const;
    bool              loadContext();
    bool              writeTransactionReport( int transactionId, std::vector<int>& fileIds, bool commitOk, bool canceled );
    bool              populateTree( bool rememberState );
    bool              populateTreeAddNode( QTreeWidget* tree, DIngestFile file, QTreeWidgetItem* parent=NULL );
    bool              hasDuplicates( const std::vector< int >& selectedIds ) const;
    void              removeDuplicates( std::vector< int >& selectedIds ) const;
    bool              createEditMetadataDialog(QString& errorMessage, DEditMetadataWindowBase * editMetadataDialog, DMetadataItemGroupList& metadataGroupList);

private:
    Ui::PiqlIngestMainWindow   m_Ui;
    DPiqlIngestConfig*         m_Config;
    DIngestFileList            m_FileList;
    int                        m_Transaction;
    DPath                      m_TempDir;
    DPath                      m_DataDir;
    DPath                      m_ReportsDir;
    DPiqlIngestProgressDialog* m_ProgressDialog;
    std::string                m_CommitMessage;
    int                        m_CommitStatus;
    bool                       m_CommitCanceled;
    FileTreeMap                m_FileTreeMap;
    DMetadataItemGroupList     m_PackageMetadata;

    friend class WorkerThread;
};

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    WorkerThread( DPiqlIngestMainWindow* mainDialog, const std::string& command );
    bool progress( int& progress, std::string& message );

protected:
    DPiqlIngestMainWindow* m_MainDialog;
    QMutex                 m_Mutex;
    bool                   m_HasProgress;
    std::string            m_Message;
    int                    m_Progress;

private: 
    void run() Q_DECL_OVERRIDE;
    static bool updateProgress( std::string& progress, void* data );
    static bool cancelExecution( void* data );

private:
    std::string            m_Command;
    
};



D_NAMESPACE_END

#endif /* DPIQLINGESTMAINWINDOW_H */
