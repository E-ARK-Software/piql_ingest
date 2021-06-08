/***************************************************************************
**
**  Implementation of the DEditMetadataWindow class
**
**  Creation date:  2017/03/30
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
#include "deditmetadatawindowbase.h"
#include "dphputils.h"
//#include "piql_ingest_config.h"

//  BASE INCLUDES
//
#include "dsystemtools.h"

//  QT INCLUDES
//
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QEvent>
#include <QKeyEvent>

#define UNUSED_PARAMETER(parameter) do { (void)(parameter); } while (0)

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )

/****************************************************************************/
/*! \class DEditMetadataWindowBase deditmetadatawindowbase.h
 *  \ingroup PiqlIngest
 *  \brief Base class for editing metadata dialog
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

//----------------------------------------------------------------------------
/*! \fn DEditMetadataWindowBase::DEditMetadataWindowBase()
 *
 *  Constructor
 */

 DEditMetadataWindowBase::DEditMetadataWindowBase(QWidget * parent, const DPath& tempDir, DPiqlIngestConfig * config) :
 QDialog(parent),
 m_Initializing(false),
 m_TempDir(tempDir),
 m_Config(config)
{
}


//----------------------------------------------------------------------------
/*! \fn DEditMetadataWindowBase::~DEditMetadataWindowBase()
 *
 *  Destructor
 */

DEditMetadataWindowBase::~DEditMetadataWindowBase()
{
}


//----------------------------------------------------------------------------
/*! \fn DEditMetadataWindowBase::validate()
 *
 *  Validate input
 */

bool DEditMetadataWindowBase::validate() const
{
    ERROR_F( "DEditMetadataWindow::validate" );
    
    if ( hasError().length() )
    {
        error << ERRerror << "Dialog has errors" << endl;
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
/*! \fn DEditMetadataWindowBase::showForm()
*
*  Show form
*/

void DEditMetadataWindowBase::showForm()
{
    ERROR_F( "DEditMetadataWindow::showForm" );

    if ( m_CurrentForm >= m_Forms.size() )
    {
        stringstream text;
        text << "Invalid form index: " << m_CurrentForm;
        setError( text.str() );
        return;
    }
    
    // Set layout for dialog
    m_StackedWidget->setCurrentIndex( m_CurrentForm );
}


//----------------------------------------------------------------------------
/*! \fn DEditMetadataWindowBase::hasError()
 *
 *  Check error state for dialog
 */

string DEditMetadataWindowBase::hasError() const
{
    return m_HasError;
}


//----------------------------------------------------------------------------
/*! \fn DEditMetadataWindowBase::clearError()
 *
 *  Clear dialog error state
 */

void DEditMetadataWindowBase::clearError()
{
    m_HasError.clear();
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


bool DEditMetadataWindowBase::FillStackedWidget(const std::string& phpBinPath)
{
    UNUSED_PARAMETER(phpBinPath);
    return true;
}


//----------------------------------------------------------------------------
/*!
 *  \brief All necessary settings are performed when creating a new instance of the class.
 *
 *  All necessary settings are performed when creating a new instance of the class.
 *  A metadata input object is generated and form control components are configured.
 *
 *  \param[in]  phpBinPath  The path to the executable php file.
 */

void DEditMetadataWindowBase::ConstructorSetup(const std::string& phpBinPath)
{
    ERROR_F("DEditMetadataWindowBase::DEditMetadataWindowBase");

    m_Ui.setupUi(this);

    // Set current file
    m_CurrentForm = 0;

    // Build metadata forms
    m_StackedWidget = new QStackedWidget();

    if (!FillStackedWidget(phpBinPath))
    {
        return;
    }

    // Set dialog layout
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 10, 0, 0);
    mainLayout->addWidget(m_StackedWidget);
    setLayout(mainLayout);

    // Resize dialog
    if (m_Forms.size())
    {
        // Tabs
        for (unsigned int i = 0; i < m_Forms.size(); i++)
        {
            for (int j = 0; j < m_Forms[i].m_TabWidget->count(); j++)
            {
                m_Forms[i].m_TabWidget->widget(j)->adjustSize();
            }
        }

        // Dialog
        adjustSize();
    }

    // Disable manual resizing
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    // Hide help button
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Set focus to first input element
    if (m_TabOrderManager.item(0))
    {
        m_TabOrderManager.item(0)->setFocus();
    }

    showForm();
}


//----------------------------------------------------------------------------
/*!
 *  \brief Read all metadata groups and metadata items.
 *
 *  We read all metadata groups and metadata items from the given php file.
 *  In this case, the type of the item and its default value are also read.
 *
 *  \param[out] metadataTemplate  The template object in which we save all the read data.
 *  \param[in]  phpBinPath        The path to the executable php file.
 *  \param[in]  scriptName        The path to the script from which we will read data.
 *  \param[in]  ingestFileName    The name of the file to which the metadata relates. Default is empty.
 *  \return Returns true if successful.
 */

bool DEditMetadataWindowBase::ReadMetadataTemplate(DMetadataTemplate& metadataTemplate, const std::string& phpBinPath, const std::string scriptName, const std::string ingestFileName)
{
    ERROR_F("DEditMetadataWindow::DEditMetadataWindow");

    string metadataTemplateFile = DPhpUtils::GetScriptPath(scriptName);
    if (!metadataTemplateFile.length())
    {
        setError("File does not exist: " + metadataTemplateFile);
        return false;
    }
    
    if (DPath(metadataTemplateFile).extension() == "xml")
    {
        if (!metadataTemplate.read(metadataTemplateFile))
        {
            setError("Failed to read metadata template: " + metadataTemplateFile);
            return false;
        }

        return true;
    }
    else if (DPath(metadataTemplateFile).extension() == "php")
    {
        DPath contextPath;
        DScriptContext context;

        if (ingestFileName.length() > 0)
        {
            context.addParameterValue("FilePaths", ingestFileName);
        }

        if (!DPhpUtils::CreateScriptContext(contextPath, context, m_TempDir))
        {
            setError("Failed to create context");
            return false;
        }
        else
        {
            // Execute script: script.php <fileName>
            const string command = phpBinPath + " -f \"" + metadataTemplateFile + "\" \"" + m_Config->applicationLanguage() + "\" \"" + contextPath.path() + "\"";
            error << ERRinfo << "Command=" << command << endl;
            string definition;
            if (!DSystemTools::doShellCommand(definition, command))
            {
                setError("Failed to execute command: " + command);
                return false;
            }

            istringstream definitionStream(definition);
            if (!metadataTemplate.read(definitionStream))
            {
                setError("Failed to read metadata template: " + definition);
                return false;
            }
        }

        // Delete context
        if (!DPath::deleteFile(contextPath))
        {
            error << ERRerror << "Failed to delete context: " << contextPath.path();
        }

        return true;
    }
    else
    {
        setError("Metadata template has invalid extension: " + DPath(metadataTemplateFile).extension());
        return false;
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Fills the metadata input object according to the template.
 *
 *  Reads information about metadata groups and the composition of metadata groups from the metadata template object.
 *  According to this information, an object for navigating through the groups and the contents of the metadata input groups are formed.
 *
 *  \param[in]  metadataTemplate         The template object that contains all metadata information.
 *  \param[in]  initialMetadataGroupList Contains information about current metadata values, in case of editing existing metadata.
 *  \param[in]  layout                   The layer in which we save the created metadata editing widget.
 *  \param[in]  formIndex                Current form index.
 *  \return Returns true if successful.
 */

bool DEditMetadataWindowBase::CreateMetadataForm(const DMetadataTemplate& metadataTemplate, const DMetadataItemGroupList& initialMetadataGroupList, QVBoxLayout * layout, int formIndex)
{
    // Create file metadata form
    m_FileMetadataTemplateGroupList = metadataTemplate.fileMetadataGroups();

    vector<QLabel*> labels;
    int labelWidth = -1;
    QTabWidget* metadataTabWidget = new QTabWidget(this);
    metadataTabWidget->tabBar()->setUsesScrollButtons(true);

    for (unsigned int i = 0; i < m_FileMetadataTemplateGroupList.groupCount(); i++)
    {
        // Get template metadata item group
        DMetadataTemplateItemGroup group;
        if (!m_FileMetadataTemplateGroupList.getGroup(group, i))
        {
            setError("Metadata template error (get group error)");
            return false;
        }

        // Create group layout item
        if (!CreateGroupLayoutItem(group, initialMetadataGroupList, labels, metadataTabWidget, labelWidth, formIndex, i))
        {
            setError("Create group layout item error");
            return false;
        }
    }

    layout->addWidget(metadataTabWidget);
    m_Forms[m_Forms.size() - 1].m_TabWidget = metadataTabWidget;

    // Adjust size of labels - avoid cutting part of the text on word-wrap
    for (unsigned int j = 0; j < labels.size(); j++)
    {
        labels[j]->setFixedSize(QSize(labelWidth, labels[j]->heightForWidth(labelWidth)));
    }

    return true;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Set dialog in error state.
 *
 *  Set dialog in error state.
 *
 *  \param[in]  errorText Error text.
 */

void DEditMetadataWindowBase::setError(const string& errorText)
{
    ERROR_F( "DEditMetadataWindow::setError" );
    
    error << ERRerror << errorText << endl;
    m_HasError = errorText;
}


//----------------------------------------------------------------------------
/*!
 *  \brief A form value has been changed.
 *
 *  A form value has been changed.
 *
 *  \param[in]  init Sign of form initialization.
 */

void DEditMetadataWindowBase::editAllFormValueEdited(bool init)
{
    ERROR_F( "DEditMetadataWindow::editAllFormValueEdited" );

    // Initialize form
    if ( init && m_CurrentForm == 0 )
    {
        m_Initializing = true;
        for ( unsigned int i = 0; i < m_Forms[0].m_GuiItems.size(); i++ )
        {
            // Check if the value is equal in all forms
            bool allValuesEqual = true;
            for ( unsigned int j = 2; j < m_Forms.size(); j++ )
            {
                if ( m_Forms[j].m_GuiItems[i].value() != m_Forms[j - 1].m_GuiItems[i].value() )
                {
                    allValuesEqual = false;
                }
            }

            // Check if item is read-only
            bool readOnly = m_Forms[0].m_GuiItems[i].readOnly();

            // Set color depending on state
            if ( readOnly )
            {
                // Do nothing
            }
            else if ( allValuesEqual )
            {
                // Set color
                m_Forms[0].m_GuiItems[i].markConsistent( true );
            }
            else
            {
                // Set color
                m_Forms[0].m_GuiItems[i].markConsistent( false );
            }

            // Set value
            if ( allValuesEqual )
            {
                // Set value
                m_Forms[0].m_GuiItems[i].setValue( m_Forms[1].m_GuiItems[i].value() );
            }
            else
            {
                // Reset
                m_Forms[0].m_GuiItems[i].reset();
            }
            m_Forms[0].m_GuiItems[i].m_OldValue = m_Forms[0].m_GuiItems[i].value();
        }
        m_Initializing = false;
    }
    else if ( !m_Initializing && m_CurrentForm == 0 )
    {
        // Update changed values in all forms
        for ( unsigned int i = 0; i < m_Forms[0].m_GuiItems.size(); i++ )
        {
            // Check if value has changed
            if ( m_Forms[0].m_GuiItems[i].visible() && !m_Forms[0].m_GuiItems[i].readOnly() && m_Forms[0].m_GuiItems[i].value() != m_Forms[0].m_GuiItems[i].m_OldValue) // Handle visiblity and readonly
            {
                // Update all forms
                for ( unsigned int j = 1; j < m_Forms.size(); j++ )
                {
                    m_Forms[j].m_GuiItems[i].setValue( m_Forms[0].m_GuiItems[i].value() );
                }

                // Remember new value
                m_Forms[0].m_GuiItems[i].m_OldValue = m_Forms[0].m_GuiItems[i].value();

                // Set color
                m_Forms[0].m_GuiItems[i].markConsistent( true );
            }
        }
    }
}


//===================================
//  P R I V A T E   I N T E R F A C E
//===================================


//----------------------------------------------------------------------------
/*!
 *  \brief Create an object for editing a metadata group.
 *
 *  Create an object for editing a metadata group
 *  and fill it according to the contents of the current group.
 *
 *  \param[in]  group                    The template of the current group of metadata.
 *  \param[in]  initialMetadataGroupList Contains information about current metadata values.
 *  \param[in]  labels                   Labels vector.
 *  \param[in]  metadataTabWidget        Metadata tab widget.
 *  \param[in]  labelWidth               Max label width.
 *  \param[in]  formIndex                Current form index.
 *  \param[in]  groupIndex               Current group index.
 *  \return Returns true if successful.
 */

bool DEditMetadataWindowBase::CreateGroupLayoutItem(DMetadataTemplateItemGroup& group,
    const DMetadataItemGroupList& initialMetadataGroupList,
    vector<QLabel*>& labels,
    QTabWidget* metadataTabWidget,
    int& labelWidth,
    unsigned int formIndex,
    unsigned int groupIndex)
{
    // Create group layout item
    QVBoxLayout * metadataLayout = new QVBoxLayout();
    QGroupBox * metadataGroupBox = new QGroupBox("");

    // Minimize space usage
    metadataGroupBox->setFlat(true);
    metadataGroupBox->setStyleSheet("QGroupBox{padding-top:15px; margin-top:-15px}");

    bool visibleItemsAdded = false;
    for (unsigned int k = 0; k < group.itemCount(); k++)
    {
        if (!CreateGuiItem(group, initialMetadataGroupList, labels, labelWidth, metadataLayout, formIndex, groupIndex, k, visibleItemsAdded))
        {
            setError("Create gui item error");
            return false;
        }
    }

    QSpacerItem* verticalSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    metadataLayout->addSpacerItem(verticalSpacer);
    metadataLayout->addStretch(1);
    metadataGroupBox->setLayout(metadataLayout);
    metadataGroupBox->adjustSize();

    // Add group to layout

    int tabIndex = metadataTabWidget->addTab(metadataGroupBox, QString::fromStdString(group.description()));
    metadataTabWidget->setTabToolTip(tabIndex, QString::fromStdString(group.description()));

    return true;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create an object for editing a metadata item.
 *
 *  Create an object for editing a metadata item
 *  and fill it according to the contents of the current item.
 *
 *  \param[in]  group                    The template of the current group of metadata.
 *  \param[in]  initialMetadataGroupList Contains information about current metadata values.
 *  \param[in]  labels                   Labels vector.
 *  \param[in]  labelWidth               Max label width.
 *  \param[in]  metadataLayout           A layer containing all the objects in a metadata group.
 *  \param[in]  formIndex                Current form index.
 *  \param[in]  groupIndex               Current group index.
 *  \param[in]  itemIndex                Current item index.
 *  \param[in]  visibleItemsAdded        Sign that added visible item.
 *  \return Returns true if successful.
 */

bool DEditMetadataWindowBase::CreateGuiItem(DMetadataTemplateItemGroup& group, const DMetadataItemGroupList& initialMetadataGroupList, vector<QLabel*>& labels, int& labelWidth, QVBoxLayout * metadataLayout, unsigned int formIndex, unsigned int groupIndex, unsigned int itemIndex, bool& visibleItemsAdded)
{
    // Create gui item
    DEditMetadataWindowGuiItem guiItem;

    // Set group info
    guiItem.m_GroupIndex = groupIndex;
    guiItem.m_GroupName = group.name();

    // Get template metadata item
    DMetadataTemplateItem item;
    if (!group.getItem(item, itemIndex))
    {
        setError("Metadata template error (get item error)");
        return false;
    }

    guiItem.m_MetadataName = item.name();

    // Get metadata
    string metadata = GetMetadata(initialMetadataGroupList, item, groupIndex, itemIndex);

    // Create item layout
    QHBoxLayout * itemLayout = new QHBoxLayout();

    // Add label
    QLabel * label = CreateLabelItem(itemLayout, item, labels, labelWidth);
    if (!label)
    {
        setError("Get label element error");
        return false;
    }

    // Add input field
    guiItem.m_InputElement = CreateInputFieldItem(itemLayout, item, metadata, formIndex);
    if (!guiItem.m_InputElement)
    {
        setError("Get input field element error");
        return false;
    }

    // Set readonly
    guiItem.setReadOnly(item.readOnly());
    if (!guiItem.readOnly())
    {
        guiItem.markConsistent(true);
    }

    // Set visibility
    if (item.visible())
    {
        // Add some space between rows
        if (visibleItemsAdded)
        {
            QSpacerItem* verticalRowSpacer = new QSpacerItem(1, 5, QSizePolicy::Expanding, QSizePolicy::Fixed);
            metadataLayout->addSpacerItem(verticalRowSpacer);
        }
        visibleItemsAdded = true;

        // Make visible
        guiItem.setVisible(true);

        // Add to dialog
        metadataLayout->addLayout(itemLayout);

        // Set tab order
        if (!item.readOnly())
        {
            m_TabOrderManager.setTabOrder(guiItem.m_InputElement, groupIndex, formIndex);
        }
    }
    else
    {
        // Make invisible
        guiItem.setVisible(false);
        label->setVisible(false);
        guiItem.m_InputElement->setVisible(false);
    }

    // Set font size
    if (item.fontSize() > 0)
    {
        guiItem.setFontSize(item.fontSize());
    }

    // Store gui item
    m_Forms[m_Forms.size() - 1].m_GuiItems.push_back(guiItem);

    return true;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Gets the initial metadata value.
 *
 *  Gets the initial metadata value
 *  from the initial metadata group list.
 *
 *  \param[in]  initialMetadataGroupList Contains information about current metadata values.
 *  \param[in]  item                     Metadata template item.
 *  \param[in]  groupIndex               Current group index.
 *  \param[in]  itemIndex                Current item index.
 *  \return Returns the value of the current metadata item.
 */

std::string DEditMetadataWindowBase::GetMetadata(const DMetadataItemGroupList& initialMetadataGroupList, const DMetadataTemplateItem& item, unsigned int groupIndex, unsigned int itemIndex)
{
    // Get metadata
    string metadata;
    if (initialMetadataGroupList.groupCount() > 0)
    {
        DMetadataItemGroup metadataItemGroup;
        if (!initialMetadataGroupList.getGroup(metadataItemGroup, groupIndex))
        {
            setError("Metadata template error");
            return "";
        }

        DMetadataItem metadataItem;
        if (metadataItemGroup.getItem(metadataItem, itemIndex))
        {
            metadata = metadataItem.value();
        }
        else
        {
            setError("Metadata template error");
            return "";
        }
    }
    else
    {
        metadata = item.defaultValue();
    }

    return metadata;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a label for the metadata item.
 *
 *  Creates a label object for the current metadata item.
 *
 *  \param[in]  itemLayout A layer containing objects of the current item.
 *  \param[in]  item       Metadata template item.
 *  \param[in]  labels     Labels vector.
 *  \param[in]  labelWidth Max label width.
 *  \return Returns new label object.
 */

QLabel * DEditMetadataWindowBase::CreateLabelItem(QHBoxLayout * itemLayout, DMetadataTemplateItem& item, vector<QLabel*>& labels, int& labelWidth)
{
    // Add label
    QHBoxLayout * labelLayout = new QHBoxLayout();
    QLabel * label = new QLabel(QString::fromStdString(item.description()));

    // Disable automatic wrapping, needs to be stated explicit with \n or <br>
    label->setWordWrap(false);

    if (item.labelFontSize() > 0)
    {
        label->setStyleSheet(QString("font: %1px;").arg(item.labelFontSize()));
    }
    if (label->minimumSizeHint().width() > labelWidth)
    {
        labelWidth = label->minimumSizeHint().width();
    }
    labels.push_back(label);
    labelLayout->addWidget(label);
    itemLayout->addLayout(labelLayout);

    return label;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create an input field for the metadata item.
 *
 *  Create an input field for the metadata item.
 *
 *  \param[in]  itemLayout A layer containing objects of the current item.
 *  \param[in]  item       Metadata template item.
 *  \param[in]  metadata   Current metadata value.
 *  \param[in]  formIndex  Form index.
 *  \return Returns new input filed object.
 */

QWidget * DEditMetadataWindowBase::CreateInputFieldItem(QHBoxLayout * itemLayout, DMetadataTemplateItem& item, string metadata, unsigned int formIndex)
{
    // Add input field
    QWidget * inputElement = NULL;
    switch (item.type())
    {
    case DMetadataTemplateItem::METADATA_TEMPLATE_ITEM_TYPE_STRING:
    {
        CreateTextInputField(item, &inputElement, metadata, formIndex);
        break;
    }
    case DMetadataTemplateItem::METADATA_TEMPLATE_ITEM_TYPE_DROPDOWN:
    {
        CreateDropDownInputField(item, &inputElement, metadata, formIndex);
        break;
    }
    case DMetadataTemplateItem::METADATA_TEMPLATE_ITEM_TYPE_DATETIME:
    {
        CreateDateTimeInputField(item, &inputElement, metadata, formIndex);
        break;
    }
    default:
        setError("Illegal item type");
        return NULL;
    }

    QHBoxLayout* elementLayout = new QHBoxLayout();
    elementLayout->addWidget(inputElement);
    itemLayout->addLayout(elementLayout);

    return inputElement;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a text input field for the metadata item.
 *
 *  Create a text input field for the metadata item.
 *
 *  \param[in]  item         Metadata template item.
 *  \param[in]  inputElement Input element object.
 *  \param[in]  metadata     Current metadata value.
 *  \param[in]  formIndex    Form index.
 */

void DEditMetadataWindowBase::CreateTextInputField(DMetadataTemplateItem& item, QWidget ** inputElement, string metadata, unsigned int formIndex)
{
    QLineEdit * textBox = new QLineEdit(QString::fromStdString(metadata), this);
    textBox->setFixedWidth(item.width());

    *inputElement = textBox;

    // Trigger on edit
    if (formIndex == 0)
    {
        QObject::connect(textBox, SIGNAL(textChanged(QString)), this, SLOT(formValueEdited(QString)));
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a drop down input field for the metadata item.
 *
 *  Create a drop down input field for the metadata item.
 *
 *  \param[in]  item         Metadata template item.
 *  \param[in]  inputElement Input element object.
 *  \param[in]  metadata     Current metadata value.
 *  \param[in]  formIndex    Form index.
 */

void DEditMetadataWindowBase::CreateDropDownInputField(DMetadataTemplateItem& item, QWidget ** inputElement, string metadata, unsigned int formIndex)
{
    // Get options
    vector<string> optionNames;
    vector<string> optionValues;
    if (!item.getOptions(optionNames, optionValues))
    {
        setError("Failed to get options for dropdown box");
        return;
    }

    // Populate combo box
    int currentIndex = -1;
    QComboBox * comboBox = new QComboBox(this);
    comboBox->setFixedWidth(item.width());
    for (unsigned int l = 0; l < optionNames.size(); l++)
    {
        string name = optionNames[l];
        string value = optionValues[l];

        comboBox->addItem(QString::fromStdString(name), QString::fromStdString(value));

        if (currentIndex == -1 && value == metadata)
        {
            currentIndex = l;
        }
    }

    // Select default
    if (currentIndex != -1)
    {
        comboBox->setCurrentIndex(currentIndex);
    }

    // Don't allow to edit values
    comboBox->setEditable(false);

    *inputElement = comboBox;

    // Trigger on edit
    if (formIndex == 0)
    {
        QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(formValueEdited(int)));
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Create a date/time input field for the metadata item.
 *
 *  Create a date/time input field for the metadata item.
 *
 *  \param[in]  item         Metadata template item.
 *  \param[in]  inputElement Input element object.
 *  \param[in]  metadata     Current metadata value.
 *  \param[in]  formIndex    Form index.
 */

void DEditMetadataWindowBase::CreateDateTimeInputField(DMetadataTemplateItem& item, QWidget ** inputElement, string metadata, unsigned int formIndex)
{
    QDateTimeEdit * dateBox = new QDateTimeEdit(this);
    dateBox->setFixedWidth(item.width());

    // Set format
    string format;
    if (item.getSetting(format, "format"))
    {
        dateBox->setDisplayFormat(QString::fromStdString(format));
    }

    // Get current max and min date
    QDate maxDate = dateBox->maximumDate();
    QTime maxTime = dateBox->maximumTime();
    QDate minDate = dateBox->minimumDate();
    QTime minTime = dateBox->minimumTime();

    // Set maximum year
    string maxDateYear;
    if (item.getSetting(maxDateYear, "maxdateyear"))
    {
        maxDate.setDate(QString::fromStdString(maxDateYear).toInt(), maxDate.month(), maxDate.day());
    }

    // Set maximum month
    string maxDateMonth;
    if (item.getSetting(maxDateMonth, "maxdatemonth"))
    {
        maxDate.setDate(maxDate.year(), QString::fromStdString(maxDateMonth).toInt(), maxDate.day());
    }

    // Set maximum day
    string maxDateDay;
    if (item.getSetting(maxDateDay, "maxdateday"))
    {
        maxDate.setDate(maxDate.year(), maxDate.month(), QString::fromStdString(maxDateDay).toInt());
    }

    // Set maximum hour
    string maxDateHour;
    if (item.getSetting(maxDateHour, "maxdatehour"))
    {
        maxTime.setHMS(QString::fromStdString(maxDateHour).toInt(), maxTime.minute(), maxTime.second());
    }

    // Set maximum minute
    string maxDateMinute;
    if (item.getSetting(maxDateMinute, "maxdateminute"))
    {
        maxTime.setHMS(maxTime.hour(), QString::fromStdString(maxDateMinute).toInt(), maxTime.second());
    }

    // Set maximum second
    string maxDateSecond;
    if (item.getSetting(maxDateSecond, "maxdatesecond"))
    {
        maxTime.setHMS(maxTime.hour(), maxTime.minute(), QString::fromStdString(maxDateSecond).toInt());
    }

    // Set minimum year
    string minDateYear;
    if (item.getSetting(minDateYear, "mindateyear"))
    {
        minDate.setDate(QString::fromStdString(minDateYear).toInt(), minDate.month(), minDate.day());
    }

    // Set minimum month
    string minDateMonth;
    if (item.getSetting(minDateMonth, "mindatemonth"))
    {
        minDate.setDate(minDate.year(), QString::fromStdString(minDateMonth).toInt(), minDate.day());
    }

    // Set minimum day
    string minDateDay;
    if (item.getSetting(minDateDay, "mindateday"))
    {
        minDate.setDate(minDate.year(), minDate.month(), QString::fromStdString(minDateDay).toInt());
    }

    // Set minimum hour
    string minDateHour;
    if (item.getSetting(minDateHour, "mindatehour"))
    {
        minTime.setHMS(QString::fromStdString(minDateHour).toInt(), minTime.minute(), minTime.second());
    }

    // Set minimum minute
    string minDateMinute;
    if (item.getSetting(minDateMinute, "mindateminute"))
    {
        minTime.setHMS(minTime.hour(), QString::fromStdString(minDateMinute).toInt(), minTime.second());
    }

    // Set minimum second
    string minDateSecond;
    if (item.getSetting(minDateSecond, "mindatesecond"))
    {
        minTime.setHMS(minTime.hour(), minTime.minute(), QString::fromStdString(minDateSecond).toInt());
    }

    // Set max and min date
    dateBox->setMaximumDateTime(QDateTime(maxDate, maxTime));
    dateBox->setMinimumDateTime(QDateTime(minDate, minTime));

    // Enable calendar
    bool enableCalendar;
    if (item.getSetting(enableCalendar, "enablecalendar"))
    {
        dateBox->setCalendarPopup(enableCalendar);
    }

    // Set default value
    if (metadata.size())
    {
        dateBox->setDateTime(QDateTime::fromString(QString::fromStdString(metadata), dateBox->displayFormat()));
    }

    *inputElement = dateBox;

    // Trigger on edit
    if (formIndex == 0)
    {
        QObject::connect(dateBox, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(formValueEdited(QDateTime)));
    }
}


//----------------------------------------------------------------------------
/*!
 *  \brief Filter on events.
 *
 *  Filter on events.
 *
 *  \param[in]  object Current object.
 *  \param[in]  event  Current event.
 *  \return Returns true if successful.
 */

bool DEditMetadataWindowBase::eventFilter( QObject* object, QEvent* event )
{
    ERROR_F( "DEditMetadataWindow::eventFilter" );

    if ( event->type() == QEvent::KeyPress )
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );
        if ( keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab )
        {
            // Handle tab order
            QWidget* widget = dynamic_cast<QWidget*>( object );
            if ( widget )
            {
                unsigned int tabIndex;
                unsigned int formIndex;
                QWidget* to = NULL;
                if ( keyEvent->key() == Qt::Key_Tab )
                {
                    to = m_TabOrderManager.getNextElement( tabIndex, formIndex, widget );
                }
                else
                {
                    to = m_TabOrderManager.getPreviousElement( tabIndex, formIndex, widget );
                }
                if ( to )
                {
                    // Activate file
                    m_CurrentForm = formIndex;
                    //showForm();
                    
                    // Activate tab
                    QTabWidget* tabWidget = m_Forms[m_CurrentForm].m_TabWidget;
                    if ( tabWidget->currentIndex() != (int)tabIndex )
                    {
                        tabWidget->setCurrentIndex( tabIndex );
                    }

                    // Set focus to new element
                    to->setFocus();
                }

                return true;
            }
        }
    }

    return QDialog::eventFilter( object, event );
}


//----------------------------------------------------------------------------
/*!
 *  \brief A form string value has been changed.
 *
 *   A form string value has been changed.
 *
 *  \param[in]  QString String value.
 */

void DEditMetadataWindowBase::formValueEdited(QString)
{
    editAllFormValueEdited( false );
}


//----------------------------------------------------------------------------
/*!
 *  \brief A form integer value has been changed.
 *
 *   A form integer value has been changed.
 *
 *  \param[in]  int Int value.
 */

void DEditMetadataWindowBase::formValueEdited(int)
{
    editAllFormValueEdited( false );
}


//----------------------------------------------------------------------------
/*!
 *  \brief A form date/time value has been changed.
 *
 *   A form date/time value has been changed.
 *
 *  \param[in]  QDateTime Date/time value.
 */

void DEditMetadataWindowBase::formValueEdited(QDateTime)
{
    editAllFormValueEdited( false );
}
