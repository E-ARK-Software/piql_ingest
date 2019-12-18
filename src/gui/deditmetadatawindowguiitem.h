#ifndef DEDITMETADATAWINDOWGUIITEM_H
#define DEDITMETADATAWINDOWGUIITEM_H

/*****************************************************************************
**
**  Definition of the DEditMetadataWindowGuiItem class
**
**  Creation date:  2019/11/08
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
#include "dcommon.h"
#include "dpath.h"

//  QT INCLUDES
//
#include <QWidget>

//  NAMESPACE
//
D_NAMESPACE_BEGIN(D_NAMESPACE)

//============================================================================
// CLASS: DEditMetadataWindowGuiItem
class DEditMetadataWindowGuiItem
{
public:
    DEditMetadataWindowGuiItem();

    std::string value();

    void reset();
    void markConsistent(bool isConsistent);
    
    bool readOnly();
    bool visible();

    void setValue(const std::string value);
    void setFontSize(int fontSize);
    void setReadOnly(bool readOnly);
    void setVisible(bool visible);

public:
    std::string  m_MetadataName;
    std::string  m_OldValue;
    QWidget*     m_InputElement;
    bool         m_Visible;
    bool         m_Consistent;
    int          m_FontSize;
    int          m_GroupIndex;
    std::string  m_GroupName;

private:
    bool isQLineEdit();
    bool isQComboBox();
    bool isQDateTimeEdit();

    void update();
};

D_NAMESPACE_END

#endif /* DEDITMETADATAWINDOWGUIITEM_H */
