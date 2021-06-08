/*****************************************************************************
**
**  Implementation Qt error stream functions
**
**  Creation date:  2013-09-09
**  Created by:     Ole Liabo
**
**
**
**  Copyright (c) 2013 Piql. All rights reserved.
**
**  This file is part of the "Base" application.
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "dqterror.h"

//  SYSTEM INCLUDES
//

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )


//============================================================================
//  P R I V A T E   S T A T I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*!
 *  Translate Qt messages to DError stream
 */

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
static void appMessageHandler(QtMsgType qtType, const QMessageLogContext &, const QString &msg)
#else
static void appMessageHandler( QtMsgType qtType, const char *msg ) 
#endif
{
    DError::Type type;

    switch ( qtType ) 
    {
    case QtDebugMsg:
        type = DError::TYPE_DEBUG;
        break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    case QtInfoMsg:
        type = DError::TYPE_INFO;
        break;
#endif
    case QtWarningMsg:
        type = DError::TYPE_WARNING;
        break;
    case QtCriticalMsg:
        type = DError::TYPE_ERROR;
        break;
    case QtFatalMsg:
        type = DError::TYPE_FATAL;
        break;
    default:
        type = DError::TYPE_DEBUG;
        break;
    }
    
    ERROR_F("Qt log");

    error << type << msg << endl;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#define DQtInstallMessageHandler qInstallMessageHandler
#else
#define DQtInstallMessageHandler qInstallMsgHandler
#endif

/****************************************************************************/
/*! \class DQtError dqterror.h
 *  \brief Class for sending Qt log messages to DErrorMan 
 *  \ingroup qtools
 *
 *  Registers a custom Qt message handler that forwards the Qt messages to 
 *  DErrorMan. 
 */

//============================================================================
//  S T A T I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*!
 *  Register message handler
 */

void DQtError::registerMessageHandler()
{
    DQtInstallMessageHandler(&appMessageHandler);
}


//----------------------------------------------------------------------------
/*!
 *  Unregister message handler
 */

void DQtError::unregisterMessageHandler()
{
    DQtInstallMessageHandler(NULL);
}





//============================================================================
//  G L O B A L   I N T E R F A C E
//============================================================================

DError& D_NAMESPACE::operator<<( DError& err, const QString& type )
{
    err << type.toStdString();
    return err;
}

DError& D_NAMESPACE::operator<<( DError& err, const QStringList& type )
{
    string ret;
    for (int i = 0; i < type.size(); i++)
    {
        if (i != type.size() - 1)
        {
            ret += ", ";
        }
        ret += type[i].toStdString();
    }

    err << ret;
    return err;
}

DError& D_NAMESPACE::operator<<( DError& err, const QByteArray& type )
{
    err << QString(type).toStdString();
    return err;
}

DError& D_NAMESPACE::operator<<( DError& err, const qint64 type )
{
    err << QString::number(type);
    return err;
}

DError& D_NAMESPACE::operator<<( DError& err, const QDir& type )
{
    err << type.path();
    return err;
}
