/***************************************************************************
**
**  Implementation of the DEditMetadataWindowGuiItem class
**
**  Creation date:  2017/03/30
**  Created by:     Matti Wennberg
**  Moved to separate file: Oleksandr Ivanov
**
**
**  Copyright (c) 2019 Piql AS. All rights reserved.
**
**  This file is part of the ingest tool application
**
***************************************************************************/

//  PROJECT INCLUDES
//
#include "deditmetadatawindowguiitem.h"

//  QT INCLUDES
//
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )

/****************************************************************************/
/*! \class DEditMetadataWindowGuiItem deditmetadatawindowguiitem.h
 *  \ingroup PiqlIngest
 *  \brief Class for working with metadata gui item
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

DEditMetadataWindowGuiItem::DEditMetadataWindowGuiItem(): 
    m_Consistent(true),
    m_FontSize(-1),
    m_GroupIndex(-1)
{};

std::string DEditMetadataWindowGuiItem::value()
{
    std::string value;
    if (isQLineEdit())
    {
        value = dynamic_cast<QLineEdit*>(m_InputElement)->text().toStdString();
    }
    else if (isQComboBox())
    {
        QComboBox* comboBox = dynamic_cast<QComboBox*>(m_InputElement);
        value = comboBox->itemData(comboBox->currentIndex()).toString().toStdString();
    }
    else if (isQDateTimeEdit())
    {
        QDateTimeEdit* dateBox = dynamic_cast<QDateTimeEdit*>(m_InputElement);
        value = dateBox->dateTime().toString(dateBox->displayFormat()).toStdString();
    }

    return value;
};

void DEditMetadataWindowGuiItem::setValue(const std::string value)
{
    if (isQLineEdit())
    {
        dynamic_cast<QLineEdit*>(m_InputElement)->setText(QString::fromStdString(value));
    }
    else if (isQComboBox())
    {
        QComboBox* comboBox = dynamic_cast<QComboBox*>(m_InputElement);
        for (int i = 0; i < comboBox->count(); i++)
        {
            if (comboBox->itemData(i).toString().toStdString() == value)
            {
                comboBox->setCurrentIndex(i);
                break;
            }
        }
    }
    else if (isQDateTimeEdit())
    {
        dynamic_cast<QDateTimeEdit*>(m_InputElement)->setDateTime(QDateTime::fromString(QString::fromStdString(value), dynamic_cast<QDateTimeEdit*>(m_InputElement)->displayFormat()));
    }
};

void DEditMetadataWindowGuiItem::reset()
{
    if (isQLineEdit())
    {
        dynamic_cast<QLineEdit*>(m_InputElement)->setText("");
    }
    else if (isQComboBox())
    {
        QComboBox* comboBox = dynamic_cast<QComboBox*>(m_InputElement);
        comboBox->setCurrentIndex(0);
    }
    else if (isQDateTimeEdit())
    {
        QDateTimeEdit* dateBox = dynamic_cast<QDateTimeEdit*>(m_InputElement);
        dateBox->setDateTime(QDateTime());
    }
};

void DEditMetadataWindowGuiItem::markConsistent(bool isConsistent)
{
    m_Consistent = isConsistent;
    update();
}

void DEditMetadataWindowGuiItem::setFontSize(int fontSize)
{
    m_FontSize = fontSize;
    update();
}

bool DEditMetadataWindowGuiItem::readOnly()
{
    bool readOnly = true;
    if (isQLineEdit())
    {
        readOnly = dynamic_cast<QLineEdit*>(m_InputElement)->isReadOnly();
    }
    else if (isQComboBox())
    {
        QComboBox* comboBox = dynamic_cast<QComboBox*>(m_InputElement);
        readOnly = !comboBox->isEnabled();
    }
    else if (isQDateTimeEdit())
    {
        QDateTimeEdit* dateBox = dynamic_cast<QDateTimeEdit*>(m_InputElement);
        readOnly = dateBox->isReadOnly();
    }
    return readOnly;
};

void DEditMetadataWindowGuiItem::setReadOnly(bool readOnly)
{
    if (isQLineEdit())
    {
        dynamic_cast<QLineEdit*>(m_InputElement)->setReadOnly(readOnly);
        dynamic_cast<QLineEdit*>(m_InputElement)->setEnabled(!readOnly);
    }
    else if (isQComboBox())
    {
        QComboBox* comboBox = dynamic_cast<QComboBox*>(m_InputElement);
        comboBox->setEnabled(!readOnly);
    }
    else if (isQDateTimeEdit())
    {
        QDateTimeEdit* dateBox = dynamic_cast<QDateTimeEdit*>(m_InputElement);
        dateBox->setReadOnly(readOnly);
        dateBox->setEnabled(!readOnly);
    }
};

bool DEditMetadataWindowGuiItem::visible()
{
    return m_Visible;
};

void DEditMetadataWindowGuiItem::setVisible(bool visible)
{
    m_Visible = visible;
};


//===================================
//  P R I V A T E   I N T E R F A C E
//===================================

bool DEditMetadataWindowGuiItem::isQLineEdit() 
{ 
    return m_InputElement->metaObject()->className() == QLineEdit().metaObject()->className(); 
};


bool DEditMetadataWindowGuiItem::isQComboBox() 
{ 
    return m_InputElement->metaObject()->className() == QComboBox().metaObject()->className(); 
};


bool DEditMetadataWindowGuiItem::isQDateTimeEdit() 
{ 
    return m_InputElement->metaObject()->className() == QDateTimeEdit().metaObject()->className(); 
};


void DEditMetadataWindowGuiItem::update()
{
    ERROR_F("DEditMetadataWindowGuiItem::update");

    if (isQLineEdit())
    {
        QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(m_InputElement);

        QString style = "QLineEdit { ";
        if (m_Consistent)
        {
            style += "background-color: white; ";
        }
        else
        {
            style += "background-color: #C8C8C8; ";
        }
        if (m_FontSize > 0)
        {
            style += QString("font-size: %1px;").arg(m_FontSize);
        }
        style += "}";

        lineEdit->setStyleSheet(style);
    }
    else if (isQComboBox())
    {
        QComboBox* comboBox = dynamic_cast<QComboBox*>(m_InputElement);

        QString style = "QComboBox { ";
        if (m_Consistent)
        {
            style += "background-color: white; ";
        }
        else
        {
            style += "background-color: #C8C8C8; ";
        }
        if (m_FontSize > 0)
        {
            style += QString("font-size: %1px;").arg(m_FontSize);
        }
        style += "}";

        comboBox->setStyleSheet(style);
    }
    else if (isQDateTimeEdit())
    {
        QDateTimeEdit* dateBox = dynamic_cast<QDateTimeEdit*>(m_InputElement);

        QString style = "QDateTimeEdit { ";
        if (m_Consistent)
        {
            style += "background-color: white; ";
        }
        else
        {
            style += "background-color: #C8C8C8; ";
        }
        if (m_FontSize > 0)
        {
            style += QString("font-size: %1px;").arg(m_FontSize);
        }
        style += "}";

        dateBox->setStyleSheet(style);
    }
}