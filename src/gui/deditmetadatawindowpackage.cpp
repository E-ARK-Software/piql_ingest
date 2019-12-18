/*****************************************************************************
**
**  Definition of the DEditMetadataWindowBase class
**
**  Creation date:  2019/25/19
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
#include "deditmetadatawindowpackage.h"

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
/*! \class DEditMetadataWindowPackage deditmetadatawindowpackage.h
 *  \ingroup PiqlIngest
 *  \brief Dialog for editing package metadata
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowPackage::DEditMetadataWindowPackage()
 *
 *  Constructor
 */

 DEditMetadataWindowPackage::DEditMetadataWindowPackage(QWidget * parent, DMetadataItemGroupList& metadataGroupList, const std::string& phpBinPath, const DPath& tempDir, DPiqlIngestConfig * config) :
    m_MetadataGroupList(metadataGroupList),
    DEditMetadataWindowBase(parent, tempDir, config)
{
    ConstructorSetup(phpBinPath);
}


//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindow::~DEditMetadataWindow()
 *
 *  Destructor
 */

DEditMetadataWindowPackage::~DEditMetadataWindowPackage()
{
}


//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowPackage::showForm()
*
*  Show form
*/

void DEditMetadataWindowPackage::showForm()
{
    DEditMetadataWindowBase::showForm();

    QString windowTitle = QString(QDialog::tr("Edit package metadata"));
    setWindowTitle(windowTitle);
}


//----------------------------------------------------------------------------
/*! \fn     DEditMetadataWindowPackage::validate()
 *
 *  Validate input
 */

bool DEditMetadataWindowPackage::validate() const
{
    if (!DEditMetadataWindowBase::validate())
    {
        return false;
    }

    ERROR_F("DEditMetadataWindowPackage::validate");

    if (!m_MetadataGroupList.isValid())
    {
        error << ERRerror << "Package metadata is invalid" << endl;
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

bool DEditMetadataWindowPackage::FillStackedWidget(const std::string& phpBinPath)
{
    m_Forms.push_back(DEditMetadataWindowForm());

    // Create a clean layout
    QWidget * widget = new QWidget();
    m_StackedWidget->addWidget(widget);
    QVBoxLayout * layout = new QVBoxLayout();
    widget->setLayout(layout);

    DMetadataTemplate metadataTemplate;
    const std::string scriptName = "metadata-description-package.php";
    if (!ReadMetadataTemplate(metadataTemplate, phpBinPath, scriptName))
    {
        setError("Error reading metadata template: " + scriptName);
        return false;
    }

    if (!CreateMetadataForm(metadataTemplate, DMetadataItemGroupList(), layout, 0))
    {
        setError("Error creating metadata form");
        return false;
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

    // Signals and slots
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPressed()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

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

void DEditMetadataWindowPackage::saveButtonPressed()
{
    ERROR_F( "DEditMetadataWindow::saveButtonPressed" );

    // Fetch metadata
    DMetadataItemGroup group;
    int currentGroupIndex = -1;
    for (unsigned int j = 0; j < m_Forms[0].m_GuiItems.size(); j++)
    {
        if (m_Forms[0].m_GuiItems[j].m_GroupIndex != currentGroupIndex)
        {
            if (currentGroupIndex >= 0)
            {
                m_MetadataGroupList.addGroup(group);
            }
            group = DMetadataItemGroup(m_Forms[0].m_GuiItems[j].m_GroupName);
            currentGroupIndex = m_Forms[0].m_GuiItems[j].m_GroupIndex;
        }

        DMetadataItem item(m_Forms[0].m_GuiItems[j].m_MetadataName, m_Forms[0].m_GuiItems[j].value());
        group.addItem(item);

        // Validate input
        unsigned int currentTemplateItemIndex = 0;
        for (unsigned int k = 0; k < m_FileMetadataTemplateGroupList.groupCount() && currentTemplateItemIndex <= j; k++)
        {
            // Get template metadata item group
            DMetadataTemplateItemGroup templateGroup;
            if (!m_FileMetadataTemplateGroupList.getGroup(templateGroup, k))
            {
                setError("Metadata template error");
                accept();
                return;
            }

            for (unsigned int l = 0; l < templateGroup.itemCount() && currentTemplateItemIndex <= j; l++)
            {
                if (currentTemplateItemIndex == j)
                {
                    // Get template metadata item
                    DMetadataTemplateItem templateItem;
                    if (!templateGroup.getItem(templateItem, l))
                    {
                        setError("Metadata template error");
                        accept();
                        return;
                    }

                    // Type specific validation
                    if (!templateItem.validate(item.value()))
                    {
                        error << ERRerror << "Type specific validation failed" << endl;
                        setError("Invalid value of item '" + templateItem.description() + "' for package");
                        accept();
                        return;
                    }

                    // Custom validation
                    string regex;
                    if (templateItem.getSetting(regex, "validateregex"))
                    {
                        if (!DRegExpUtil::Match(item.value(), regex))
                        {
                            error << ERRerror << "Custom validation failed" << endl;
                            setError("Invalid value of item '" + templateItem.description() + "' for package");
                            accept();
                            return;
                        }
                    }
                }

                currentTemplateItemIndex++;
            }
        }
    }
    m_MetadataGroupList.addGroup(group);

    accept();
}
