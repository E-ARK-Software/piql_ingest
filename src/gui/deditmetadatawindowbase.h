#ifndef DEDITMETADATAWINDOWBASE_H
#define DEDITMETADATAWINDOWBASE_H

/*****************************************************************************
**
**  Definition of the DEditMetadataWindowBase class
**
**  Creation date:  2019/11/08
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
#include "ui_deditmetadatawindow.h"
#include "deditmetadatawindowguiitem.h"
#include "deditmetadatawindowtabordermanager.h"
#include "dmetadatatemplate.h"
#include "dingestfilelist.h"
#include "piql_ingest_config.h"

//  BASE INCLUDES
//
#include "dcommon.h"
#include "dpath.h"

//  QT INCLUDES
//
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QWidget>

//  FORWARD DECLARATIONS
//
class QVBoxLayout;
class QHBoxLayout;
class QStackedWidget;
class QEvent;

//  NAMESPACE
//
D_NAMESPACE_BEGIN(D_NAMESPACE)


//============================================================================
// CLASS: DEditMetadataWindowForm
class DEditMetadataWindowForm
{
public:
    std::vector< DEditMetadataWindowGuiItem > m_GuiItems;
    QLabel*                                   m_NavigationText;
    QTabWidget*                               m_TabWidget;
};


//============================================================================
// CLASS: DEditMetadataWindowBase
class DEditMetadataWindowBase : public QDialog
{
    Q_OBJECT

public:
    DEditMetadataWindowBase(QWidget * parent, const DPath& tempDir, DPiqlIngestConfig * config);
    virtual ~DEditMetadataWindowBase();

    virtual bool validate() const;
    virtual void showForm();
    
    std::string  hasError() const;
    void         clearError();
    
    
protected:
    virtual bool FillStackedWidget(const std::string& phpBinPath);

protected:
    void ConstructorSetup(const std::string& phpBinPath);
    bool ReadMetadataTemplate(DMetadataTemplate& metadataTemplate, const std::string& phpBinPath, const std::string phpScriptName, const std::string ingestFileName = "");
    bool CreateMetadataForm(const DMetadataTemplate& metadataTemplate, const DMetadataItemGroupList& initialMetadataGroupList, QVBoxLayout * layout, int formIndex = 0);

    void setError(const std::string& errorText);
    void editAllFormValueEdited(bool init);

private:
    bool CreateGroupLayoutItem(DMetadataTemplateItemGroup& group, 
                               const DMetadataItemGroupList& initialMetadataGroupList,
                               std::vector<QLabel*>& labels,
                               QTabWidget* metadataTabWidget,
                               int& labelWidth,
                               unsigned int formIndex, 
                               unsigned int groupIndex
                               );
    bool CreateGuiItem(DMetadataTemplateItemGroup& group, 
                       const DMetadataItemGroupList& initialMetadataGroupList, 
                       std::vector<QLabel*>& labels, 
                       int& labelWidth, 
                       QVBoxLayout * metadataLayout, 
                       unsigned int formIndex, 
                       unsigned int groupIndex, 
                       unsigned int itemIndex, 
                       bool& visibleItemsAdded);
    std::string GetMetadata(const DMetadataItemGroupList& initialMetadataGroupList, const DMetadataTemplateItem& item, unsigned int groupIndex, unsigned int itemIndex);
    QLabel * CreateLabelItem(QHBoxLayout * itemLayout, DMetadataTemplateItem& item, std::vector<QLabel*>& labels, int& labelWidth);
    QWidget * CreateInputFieldItem(QHBoxLayout * itemLayout, DMetadataTemplateItem& item, std::string metadata, unsigned int formIndex);
    void CreateTextInputField(DMetadataTemplateItem& item, QWidget ** inputElement, std::string metadata, unsigned int formIndex);
    void CreateDropDownInputField(DMetadataTemplateItem& item, QWidget ** inputElement, std::string metadata, unsigned int formIndex);
    void CreateDateTimeInputField(DMetadataTemplateItem& item, QWidget ** inputElement, std::string metadata, unsigned int formIndex);
    bool eventFilter(QObject* object, QEvent* event);
    void navigateStart();

private slots:
    void formValueEdited(QString);
    void formValueEdited(int);
    void formValueEdited(QDateTime);

protected:
    Ui::EditMetadataWindow                 m_Ui;
    std::vector< DEditMetadataWindowForm > m_Forms;
    std::string                            m_HasError;
    unsigned int                           m_CurrentForm;
    QStackedWidget *                       m_StackedWidget;
    bool                                   m_Initializing;
    DPath                                  m_TempDir;
    DMetadataTemplateItemGroupList         m_FileMetadataTemplateGroupList;
    DEditMetadataWindowTabOrderManager     m_TabOrderManager;
    DPiqlIngestConfig *                    m_Config;
};


D_NAMESPACE_END

#endif /* DEDITMETADATAWINDOWBASE_H */
