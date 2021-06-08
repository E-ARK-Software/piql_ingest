/***************************************************************************
**
**  Implementation of the DEditMetadataWindowTabOrderManager class
**
**  Creation date:  2019/11/11
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
#include "deditmetadatawindowtabordermanager.h"

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )

/****************************************************************************/
/*! \class DEditMetadataWindowTabOrderManager deditmetadatawindowtabordermanager.h
 *  \ingroup PiqlIngest
 *  \brief Class for determining the order of gui items
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================

DEditMetadataWindowTabOrderManager::DEditMetadataWindowTabOrderManager() {};

void DEditMetadataWindowTabOrderManager::setTabOrder(QWidget * widget, unsigned int tabIndex, unsigned int formIndex)
{
    m_Widgets.push_back(widget);
    m_TabIndexes.push_back(tabIndex);
    m_FormIndexes.push_back(formIndex);
}

QWidget * DEditMetadataWindowTabOrderManager::getNextElement(unsigned int& tabIndex, unsigned int& formIndex, const QWidget * widget)
{
    for (unsigned int i = 0; i < m_Widgets.size(); i++)
    {
        if (m_Widgets[i] == widget)
        {
            if (i == m_Widgets.size() - 1)
            {
                tabIndex = m_TabIndexes[0];
                formIndex = m_FormIndexes[0];
                return m_Widgets[0];
            }
            else
            {
                tabIndex = m_TabIndexes[i + 1];
                formIndex = m_FormIndexes[i + 1];
                return m_Widgets[i + 1];
            }
        }
    }

    return NULL;
}

QWidget * DEditMetadataWindowTabOrderManager::getPreviousElement(unsigned int& tabIndex, unsigned int& formIndex, const QWidget * widget)
{
    for (unsigned int i = 0; i < m_Widgets.size(); i++)
    {
        if (m_Widgets[i] == widget)
        {
            if (i == 0)
            {
                tabIndex = m_TabIndexes[m_Widgets.size() - 1];
                formIndex = m_FormIndexes[m_Widgets.size() - 1];
                return m_Widgets[m_Widgets.size() - 1];
            }
            else
            {
                tabIndex = m_TabIndexes[i - 1];
                formIndex = m_FormIndexes[i - 1];
                return m_Widgets[i - 1];
            }
        }
    }

    return NULL;
}

QWidget * DEditMetadataWindowTabOrderManager::item(unsigned int index) const
{
    if (index < m_Widgets.size())
    {
        return m_Widgets[index];
    }

    return NULL;
}
