#ifndef DEDITMETADATAWINDOWTABORDERMANAGER_H
#define DEDITMETADATAWINDOWTABORDERMANAGER_H

/*****************************************************************************
**
**  Definition of the DEditMetadataWindowTabOrderManager class
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
*****************************************************************************/

//  BASE INCLUDES
//
#include    "dcommon.h"

//  QT INCLUDES
//
#include <QWidget>

//  NAMESPACE
//
D_NAMESPACE_BEGIN(D_NAMESPACE)


//============================================================================
// CLASS: DEditMetadataWindowTabOrderManager
class DEditMetadataWindowTabOrderManager
{
public:
    DEditMetadataWindowTabOrderManager();

    void setTabOrder(QWidget * widget, unsigned int tabIndex, unsigned int formIndex);

    QWidget * getNextElement(unsigned int& tabIndex, unsigned int& formIndex, const QWidget * widget);
    QWidget * getPreviousElement(unsigned int& tabIndex, unsigned int& formIndex, const QWidget * widget);
    QWidget * item(unsigned int index) const;

private:
    std::vector<QWidget*>     m_Widgets;
    std::vector<unsigned int> m_TabIndexes;
    std::vector<unsigned int> m_FormIndexes;
};

D_NAMESPACE_END

#endif /* DEDITMETADATAWINDOWTABORDERMANAGER_H */