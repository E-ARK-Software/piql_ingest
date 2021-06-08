/*****************************************************************************
**
**  Creation date:  2017-03-29
**  Created by:     Matti Wennberg
**
**  Target(s):      Linux 2.4.x / GCC 3.0
**                  Win32
**
**  Namespace:      
**
**  Copyright (c) 2017 Piql AS. All rights reserved.
**
**  This file is part of the DLFR software util modules
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "cmd_util_base.h"
#include    "gui/dpiqlingestmainwindow.h"

//  BASE INCLUDES
//
#include    "derrorman.h"
#include    "derror.h"
#include    "dqterror.h"
#include    "dtclap.h"
#include    "dfilesystemtools.h"
#include    "dsystemtools.h"

//  QT INCLUDES
//
#include    <QStandardPaths>
#include    <QDir>
#include    <QTranslator>

//  SYSTEM INCLUDES
//
#include    <memory>

//  NAMESPACE
//
D_NAMESPACE_USING( D_NAMESPACE )
D_NAMESPACE_USING( std )
D_NAMESPACE_USING( TCLAP )

/****************************************************************************/
/*! \defgroup PiqlIngest Data Ingest Tool
 *  \ingroup ClientTools
 *
 *  Ingest data files to the PPS. Allows user to add metadata before uploading 
 *  to a server. 
 */

/****************************************************************************/
/*! \class DPiqlIngest
 *  \ingroup PiqlIngest
 *
 *  Application object responsible for parsing the command line and launching 
 *  actions.
 */

// Derive from CmdUtilBase 
class DPiqlIngest : public CmdUtilBase
{
public:

    enum  ReturnCode
    {
        RETURN_CODE_SUCCESS,
        RETURN_CODE_MISSING_CONFIG_FILE
    };

    DPiqlIngest();
    ~DPiqlIngest();

protected:
    virtual bool parseArguments(CmdUtilParameters& m_cmdLine);
    virtual int  launchHandler();

private:
    auto_ptr< ValueArg<string> > m_ConfigFilenameArg;
};

DPiqlIngest::DPiqlIngest()
    : CmdUtilBase("piql_ingest", DVersion( 1, 1, 0, __DATE__ ), "Piql Ingest")
{
    m_ConfigFilenameArg = auto_ptr<ValueArg<string> >( new ValueArg<string>("", "conf", "Config file path", false, "", "File name") ); 
    addArgument(*m_ConfigFilenameArg);
}

DPiqlIngest::~DPiqlIngest()
{
}


//----------------------------------------------------------------------------
/*
 *  \param  m_cmdLine          Reference to command util parameters
 *  \return success state
 *
 *  Parse command arguments
 */

bool DPiqlIngest::parseArguments(CmdUtilParameters& /*cmdLine*/)
{
    return true;
}


int DPiqlIngest::launchHandler()
{
    ERROR_F( "DPiqlIngest::launchHandler" );

    // Initiate GUI
    QApplication * qtApp;
    int argc = 0;
    qtApp = new QApplication(argc, NULL);
    qtApp->setOrganizationName("Piql");
    qtApp->setApplicationName("piql_ingest");
    qtApp->setApplicationVersion(releaseInfo().c_str());

    // Parameter values
    string configFile = "piql_ingest.ini";
#if defined (D_OS_X)
    configFile = DSystemTools::getOsXBundleResource( configFile );
#endif
    
    if ( m_ConfigFilenameArg->isSet() )
    {
        configFile = m_ConfigFilenameArg->getValue();
    }

    // Read config file
    if ( !DFileSystemTools::isExistingPath( configFile ) )
    {
        cerr << "Config file doesn't exist: " << configFile << endl;
        return RETURN_CODE_MISSING_CONFIG_FILE;
    }

    error << ERRerror <<  QStandardPaths::writableLocation( QStandardPaths::AppDataLocation).toStdString() << endl;
    
    DPiqlIngestConfig appConfig( configFile );

    // Get log level from config, command line has precedence
    if ( !isLogLevelSet() )
    {
        DErrorMan::Instance()->setLogLevel( appConfig.logLevel() );
    }

#if defined ( D_OS_WIN32 )
    // Hide console window
    if ( appConfig.hideConsoleWindow() )
    {
        error << ERRinfo << "Hiding console" << endl;
        if (!FreeConsole())
        {
            error << ERRerror << "Failed to hide console" << endl;
        }
    }
#endif

    // Set custom style
    QString style = QString::fromStdString( appConfig.applicationStyleSheet() );
    if ( !style.isEmpty() )
    {
        qtApp->setStyleSheet(style);
    }

    // Set language
    QString language = QString::fromStdString( appConfig.applicationLanguage() );
    QTranslator translator;
    QTranslator translatorQt;
    if ( !language.isEmpty() )
    {
        QLocale::setDefault( QLocale(language) );

        // Load translation for custom components
        QString languageFilePath = QString("%1%2%3%4%5%6").arg("locale").arg(QDir::separator()).arg(language).arg(QDir::separator()).arg(language).arg(".qm");
        if ( translator.load( languageFilePath ) )
        {
            qApp->installTranslator( &translator );
        }
        else
        {
            error << ERRerror << "Failed to load language: " << languageFilePath << endl;
        }

        // Load translation for standard Qt components
        QString languageFilePathQt = QString("%1%2%3%4%5%6%7").arg("locale").arg(QDir::separator()).arg(language).arg(QDir::separator()).arg("qt_").arg(language).arg(".qm");
        if ( translatorQt.load( languageFilePathQt ) )
        {
            qApp->installTranslator( &translatorQt );
        }
        else
        {
            error << ERRerror << "Failed to load language: " << languageFilePathQt << endl;
        }
    }

    // Start GUI
    DPiqlIngestMainWindow *window;
    window = new DPiqlIngestMainWindow( &appConfig );
    window->show();
    qApp->exec();

    delete  qtApp;

    return RETURN_CODE_SUCCESS;
}

//============================================================================

int  main( int argc, char* argv[] )
{
    DPiqlIngest app;

    int retval = app.launch( argc, argv );

    return retval;
}
