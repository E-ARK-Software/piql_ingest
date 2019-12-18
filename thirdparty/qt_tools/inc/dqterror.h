#ifndef DQTERROR_H
#define DQTERROR_H

/*****************************************************************************
**
**  Creation date:  2013-09-08
**  Created by:     Ole Liabo
**
**
**  Dependencies:   -
**  Restrictions:   -
**  ToDo:           -
**
**  Copyright (c) 2013 Piql. All rights reserved.
**
**  This file is part of the "Base" library.
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include   "derror.h"


//  QT INCLUDES
//
#include   <QString>
#include   <QStringList>
#include   <QByteArray>
#include   <QDir>

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )


//  FORWARD DECLARATIONS
//

//============================================================================
// CLASS: DQtError
class DQtError
{
public:
    static void registerMessageHandler();
    static void unregisterMessageHandler();
};


//============================================================================
// GLOBALS

DError& operator<<( DError& err, const QString& type );
DError& operator<<( DError& err, const QStringList& type );
DError& operator<<( DError& err, const QByteArray& type );
DError& operator<<( DError& err, const qint64 type );
DError& operator<<( DError& err, const QDir& type );

//============================================================================

D_NAMESPACE_END

#endif /* DQTERROR_H */
