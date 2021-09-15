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
#include "deditmetadatawindowfile.h"

//  BASE INCLUDES
//
#include    "dregexp.h"

//  QT INCLUDES
//
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QToolButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )

/****************************************************************************/
/*! \class DEditMetadataWindowFile deditmetadatawindowfile.h
 *  \ingroup PiqlIngest
 *  \brief Dialog for editing files metadata
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================


//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowFile::DEditMetadataWindowFile()
 *
 *  Constructor
 */

DEditMetadataWindowFile::DEditMetadataWindowFile(QWidget * parent, DIngestFileList& ingestFiles, const std::string& phpBinPath, const DPath& tempDir, DPiqlIngestConfig * config) :
    m_IngestFiles(ingestFiles),
    DEditMetadataWindowBase(parent, tempDir, config)
{
    ConstructorSetup(phpBinPath);
}


//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindow::~DEditMetadataWindow()
 *
 *  Destructor
 */

DEditMetadataWindowFile::~DEditMetadataWindowFile()
{
}


//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindow::showForm()
*
*  Show form
*/

void DEditMetadataWindowFile::showForm()
{
    DEditMetadataWindowBase::showForm();

    ERROR_F( "DEditMetadataWindowFile::showForm" );

    if ( m_CurrentForm == 0 ) // All
    {
        // Set window title
        QString windowTitle = QString( QDialog::tr("Edit metadata - All") );
        setWindowTitle( windowTitle );

        // Reset form values
        editAllFormValueEdited( true );
    }
    else // Single file edit
    {
        // Get file
        DIngestFile ingestFile;
        if ( !m_IngestFiles.getFileByIndex(ingestFile, m_CurrentForm - 1) )
        {
            setError( "Failed to get file" );
            return;
        }

        // Set window title
        QString windowTitle = QString(QDialog::tr("Edit metadata - %1")).arg(QString::fromStdString(ingestFile.fileName(false)));
        setWindowTitle( windowTitle );
    }
}


//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowFile::validate()
 *
 *  Validate input
 */

bool DEditMetadataWindowFile::validate() const
{
    if (!DEditMetadataWindowBase::validate())
    {
        return false;
    }

    ERROR_F( "DEditMetadataWindowFile::validate" );
    
    if ( !m_IngestFiles.isValid() )
    {
        error << ERRerror << "Ingest files contain errors" << endl;
        return false;
    }

    return true;
}


//=======================================
//  P R O T E C T E D   I N T E R F A C E
//=======================================


//----------------------------------------------------------------------------
/*!
 *  \brief Fills QStackedWidget object with all necessary components.
 *
 *  Fills QStackedWidget object with all necessary components.
 *  This object contains an object for navigating metadata groups 
 *  and all metadata input fields.
 *
 *  \param[in]  phpBinPath  The path to the executable php file.
 *  \return Returns true if successful.
 */

bool DEditMetadataWindowFile::FillStackedWidget(const std::string& phpBinPath)
{
    for (unsigned int i = 0; i <= m_IngestFiles.count(); i++)
    {
        DIngestFile ingestFile;
        unsigned int fileIndex = i == 0 ? i : i - 1;

        if (!m_IngestFiles.getFileByIndex(ingestFile, fileIndex))
        {
            setError("Failed to get file from list");
            return false;
        }

        m_Forms.push_back(DEditMetadataWindowForm());

        // Create a clean layout
        QWidget * widget = new QWidget();
        m_StackedWidget->addWidget(widget);
        QVBoxLayout * layout = new QVBoxLayout();
        widget->setLayout(layout);

        // Load template
        DMetadataTemplate metadataTemplate;
        const std::string scriptName = "metadata-description.php";
        if (!ReadMetadataTemplate(metadataTemplate, phpBinPath, scriptName, ingestFile.fileName(true)))
        {
            setError("Error reading metadata template: " + scriptName);
            return false;
        }

        // Validate cached metadata - clear if it's not compatible with template
        if ( !isCompatible(ingestFile.metadataGroupList(), metadataTemplate.fileMetadataGroups()) )
        {
            ingestFile.setMetadataGroups( DMetadataItemGroupList() );
        }

        // Create form and populate default values
        if (!CreateMetadataForm(metadataTemplate, ingestFile.metadataGroupList(), layout, i))
        {
            setError("Error creating metadata form");
            return false;
        }

        // Add layout for navigation
        QHBoxLayout* navigationLayout = new QHBoxLayout();
        QToolButton* startButton = new QToolButton(this);
        QToolButton* previousButton = new QToolButton(this);
        QToolButton* nextButton = new QToolButton(this);
        QToolButton* endButton = new QToolButton(this);
        QSpacerItem* navigationSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QLabel* navigationText = new QLabel("");
        startButton->setIcon(QPixmap(":/images/gui/resources/navigate_button_start.svg"));
        startButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        startButton->setFixedSize(25, 25);
        startButton->setIconSize(QSize(20, 20));
        previousButton->setIcon(QPixmap(":/images/gui/resources/navigate_button_previous.svg"));
        previousButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        previousButton->setFixedSize(25, 25);
        previousButton->setIconSize(QSize(20, 20));
        nextButton->setIcon(QPixmap(":/images/gui/resources/navigate_button_next.svg"));
        nextButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        nextButton->setFixedSize(25, 25);
        nextButton->setIconSize(QSize(20, 20));
        endButton->setIcon(QPixmap(":/images/gui/resources/navigate_button_end.svg"));
        endButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        endButton->setFixedSize(25, 25);
        endButton->setIconSize(QSize(20, 20));
        navigationLayout->addWidget(startButton);
        navigationLayout->addWidget(previousButton);
        navigationLayout->addWidget(nextButton);
        navigationLayout->addWidget(endButton);
        navigationLayout->addItem(navigationSpacer);
        navigationLayout->addWidget(navigationText);
        layout->addLayout(navigationLayout);
        m_Forms[m_Forms.size() - 1].m_NavigationText = navigationText;

        // Set label
        if (i == 0)
        {
            navigationText->setText(QString(QLabel::tr("Edit all %1 files")).arg(m_IngestFiles.count()));
        }
        else
        {
            navigationText->setText(QString(QLabel::tr("Edit file %1 of %2")).arg(i).arg(m_IngestFiles.count()));
        }

        // Add extra layout for space
        QHBoxLayout* spaceLayout = new QHBoxLayout();
        spaceLayout->addStrut(20);
        layout->addLayout(spaceLayout);

        // Add buttons
        QDialogButtonBox* buttonBox = new QDialogButtonBox();
        buttonBox->setOrientation(Qt::Horizontal);
        QPushButton* saveButton = buttonBox->addButton(QDialogButtonBox::tr("Save"), QDialogButtonBox::AcceptRole);
        QPushButton* cancelButton = buttonBox->addButton(QDialogButtonBox::tr("Cancel"), QDialogButtonBox::RejectRole);
        layout->addWidget(buttonBox);
        saveButton->installEventFilter(this);
        cancelButton->installEventFilter(this);

        // Set tab order - go to buttons when last file is populated
        if (i == m_IngestFiles.count())
        {
            m_TabOrderManager.setTabOrder(saveButton, m_FileMetadataTemplateGroupList.groupCount() - 1, i);
            m_TabOrderManager.setTabOrder(cancelButton, m_FileMetadataTemplateGroupList.groupCount() - 1, i);
        }

        // Signals and slots
        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPressed()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        QObject::connect(startButton, SIGNAL(clicked()), this, SLOT(navigateStart()));
        QObject::connect(previousButton, SIGNAL(clicked()), this, SLOT(navigatePrevious()));
        QObject::connect(nextButton, SIGNAL(clicked()), this, SLOT(navigateNext()));
        QObject::connect(endButton, SIGNAL(clicked()), this, SLOT(navigateEnd()));
    }

    return true;
}


//===================================
//  P R I V A T E   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowFile::saveButtonPressed()
 *
 *  Ok button pressed
 */

void DEditMetadataWindowFile::saveButtonPressed()
{
    ERROR_F( "DEditMetadataWindow::saveButtonPressed" );

    for ( unsigned int i = 0; i < m_IngestFiles.count(); i++ )
    {
        // Get file
        DIngestFile ingestFile;
        if ( !m_IngestFiles.getFileByIndex(ingestFile, i) )
        {
            setError( "Failed to get file" );
            accept();
            return;
        }
        DMetadataItemGroupList metadataGroupList;

        // Fetch metadata
        DMetadataItemGroup group;
        int currentGroupIndex = -1;
        for ( unsigned int j = 0; j < m_Forms[i + 1].m_GuiItems.size(); j++ )
        {
            if ( m_Forms[i + 1].m_GuiItems[j].m_GroupIndex != currentGroupIndex )
            {
                if ( currentGroupIndex >= 0 )
                {
                    metadataGroupList.addGroup( group );
                }
                group = DMetadataItemGroup( m_Forms[i + 1].m_GuiItems[j].m_GroupName );
                currentGroupIndex = m_Forms[i + 1].m_GuiItems[j].m_GroupIndex;
            }

            DMetadataItem item( m_Forms[i + 1].m_GuiItems[j].m_MetadataName, m_Forms[i + 1].m_GuiItems[j].value() );
            group.addItem( item );

            // Validate input
            unsigned int currentTemplateItemIndex = 0;
            for ( unsigned int k = 0; k < m_FileMetadataTemplateGroupList.groupCount() && currentTemplateItemIndex <= j; k++ )
            {
                // Get template metadata item group
                DMetadataTemplateItemGroup templateGroup;
                if ( !m_FileMetadataTemplateGroupList.getGroup(templateGroup, k) )
                {
                    setError( "Metadata template error" );
                    accept();
                    return;
                }

                for ( unsigned int l = 0; l < templateGroup.itemCount() && currentTemplateItemIndex <= j; l++ )
                {
                    if ( currentTemplateItemIndex == j )
                    {
                        // Get template metadata item
                        DMetadataTemplateItem templateItem;
                        if ( !templateGroup.getItem(templateItem, l) )
                        {
                            setError( "Metadata template error" );
                            accept();
                            return;
                        }

                        // Type specific validation
                        if ( !templateItem.validate(item.value()) )
                        {
                            error << ERRerror << "Type specific validation failed" << endl;
                            setError( "Invalid value of item '" + templateItem.description() + "' for file '" + ingestFile.fileName(false) + "'" );
                            accept();
                            return;
                        }

                        // Custom validation
                        string regex;
                        if ( templateItem.getSetting(regex, "validateregex") )
                        {
                            if ( !DRegExpUtil::Match(item.value(), regex) )
                            {
                                error << ERRerror << "Custom validation failed" << endl;
                                setError( "Invalid value of item '" + templateItem.description() + "' for file '" + ingestFile.fileName(false) + "'" );
                                accept();
                                return;
                            }
                        }
                    }

                    currentTemplateItemIndex++;
                }
            }
        }
        metadataGroupList.addGroup( group );

        // Update file
        ingestFile.setMetadataGroups( metadataGroupList );
        if ( !m_IngestFiles.setFileByIndex(ingestFile, i) )
        {
            setError( "Failed to set file" );
            accept();
            return;
        }
    }

    accept();
}


//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowFile::navigatePrevious()
 *
 *  Previous button pressed
 */

void DEditMetadataWindowFile::navigatePrevious()
{
    ERROR_F( "DEditMetadataWindowFile::navigatePrevious" );

    if ( m_CurrentForm < 1 )
    {
        error << ERRerror << "Out of index: " << m_CurrentForm << " of " << m_Forms.size() << endl;
        return;
    }

    m_CurrentForm--;
    showForm();
}

//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowFile::navigateNext()
 *
 *  Next button pressed
 */

void DEditMetadataWindowFile::navigateNext()
{
    ERROR_F( "DEditMetadataWindowFile::navigateNext" );

    if ( m_CurrentForm >= m_Forms.size() - 1 )
    {
        error << ERRerror << "Out of index: " << m_CurrentForm << " of " << m_Forms.size() << endl;
        return;
    }

    m_CurrentForm++;
    showForm();
}

//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowFile::navigateEnd()
 *
 *  End button pressed
 */

void DEditMetadataWindowFile::navigateEnd()
{
    ERROR_F( "DEditMetadataWindowFile::navigateEnd" );

    m_CurrentForm = m_Forms.size() - 1;
    showForm();
}

//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowFile::navigateEnd()
 *
 *  Start button pressed.
 */

void DEditMetadataWindowFile::navigateStart()
{
    ERROR_F( "DEditMetadataWindowFile::navigateStart" );

    m_CurrentForm = 0;
    showForm();
}
