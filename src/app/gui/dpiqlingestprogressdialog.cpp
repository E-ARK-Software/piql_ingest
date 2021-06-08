/***************************************************************************
**
**  Implementation of the DPiqlIngestProgressDialog class
**
**  Creation date:  2017/05/05
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
#include    "dpiqlingestprogressdialog.h"

//  BASE INCLUDES
//
#include    "derror.h"

//  QT INCLUDES
//

//  NAMESPACE
//
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( D_NAMESPACE )

/****************************************************************************/
/*! \class DPiqlIngestProgressDialog dpiqlingestprogressdialog.h
 *  \ingroup PiqlIngest
 *  \brief Dialog for showing progress
 * 
 */
 
//===================================
//  P U B L I C   I N T E R F A C E
//===================================
        
//----------------------------------------------------------------------------
/*! 
 *  Constructor
 */

DPiqlIngestProgressDialog::DPiqlIngestProgressDialog(const QString& labelText, const QString& cancelButton, int minimum, int maximum, QWidget* parent, Qt::WindowFlags) : QProgressDialog( labelText, cancelButton, minimum, maximum, parent )
{
};

//----------------------------------------------------------------------------
/*! 
 *  Control dialog visibility
 */

void DPiqlIngestProgressDialog::setVisible( bool visible )
{
    if ( visible )
    {
        QProgressDialog::setVisible(visible);
    }
}
