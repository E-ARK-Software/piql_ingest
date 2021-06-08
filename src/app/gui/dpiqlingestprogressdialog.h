#ifndef DPIQLINGESTPROGRESSDIALOG_H
#define DPIQLINGESTPROGRESSDIALOG_H

/*****************************************************************************
**  
**  Definition of the DPiqlIngestProgressDialog class
**
**  Creation date:  2017/05/05
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

//  BASE INCLUDES
//
#include    "dcommon.h"

//  QT INCLUDES
//
#include    <QProgressDialog>

//  FORWARD DECLARATIONS
//

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )


//  FORWARD DECLARATIONS
//

//============================================================================
// CLASS: DPiqlIngestProgressDialog
class DPiqlIngestProgressDialog : public QProgressDialog
{
    Q_OBJECT

public: 
    DPiqlIngestProgressDialog(const QString& labelText, const QString& cancelButton, int minimum, int maximum, QWidget* parent = Q_NULLPTR, Qt::WindowFlags = Qt::WindowFlags());

public slots:
    void setVisible( bool visible );
};


D_NAMESPACE_END

#endif /* DPIQLINGESTPROGRESSDIALOG_H */
