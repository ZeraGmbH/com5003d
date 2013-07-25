#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QStringList>
#include <xmlconfigreader.h>
#include <zeraserver.h>

#include "com5003dglobal.h"
#include "com5003d.h"
#include "application.h"
#include "pcbserver.h"
#include "debugsettings.h"
#include "ethsettings.h"
#include "i2csettings.h"
#include "fpgasettings.h"
#include "sensesettings.h"
#include "sourcesettings.h"
#include "statusinterface.h"
#include "samplinginterface.h"
#include "systeminterface.h"
#include "systeminfo.h"
#include "senseinterface.h"
#include "sourceinterface.h"
#include "atmel.h"
#include "atmelwatcher.h"
#include "adjustment.h"

extern cApplication* app;

cATMEL* pAtmel; // we take a static object for atmel connection

cCOM5003dServer::cCOM5003dServer()
    :cPCBServer()
{

    m_pDebugSettings = 0;
    m_pETHSettings = 0;
    m_pI2CSettings = 0;
    m_pFPGAsettings = 0;
    m_pSenseSettings = 0;
    pAtmel = 0;
    m_pAtmelWatcher = 0;
    m_pStatusInterface = 0;
    m_pSystemInterface = 0;
    m_pSenseInterface = 0;
    m_pSystemInfo = 0;
    m_pAdjHandler = 0;

    m_pInitializationMachine = new QStateMachine(this);

    QState* IDLE = new QState(); // we start from here
    QState* RUN = new QState(); // here we'll do something
    QFinalState* FINISH = new QFinalState(); // and here we finish

    IDLE->addTransition(app, SIGNAL(appStarting()), RUN); //
    RUN->addTransition(this, SIGNAL(abortInit()), FINISH); // from anywhere we arrive here if some error

    QState* xmlConfiguration = new QState(RUN);
    QState* wait4Atmel = new QState(RUN);
    QState* setupServer = new QState(RUN);

    RUN->setInitialState(xmlConfiguration);
    //xmlConfiguration->addTransition(myXMLConfigReader, SIGNAL(finishedParsingXML()), wait4Atmel);

    wait4Atmel->addTransition(this, SIGNAL(atmelRunning()), setupServer);

    m_pInitializationMachine->addState(IDLE);
    m_pInitializationMachine->addState(RUN);
    m_pInitializationMachine->addState(FINISH);
    m_pInitializationMachine->setInitialState(IDLE);

    QObject::connect(xmlConfiguration, SIGNAL(entered()), this, SLOT(doConfiguration()));
    QObject::connect(wait4Atmel, SIGNAL(entered()), this, SLOT(doWait4Atmel()));
    QObject::connect(setupServer, SIGNAL(entered()), this, SLOT(doSetupServer()));
    QObject::connect(FINISH, SIGNAL(entered()), this, SLOT(doCloseServer()));

    m_pInitializationMachine->start();
    if (m_pInitializationMachine->configuration().contains(IDLE) )
        qDebug("IDLE");
}


cCOM5003dServer::~cCOM5003dServer()
{
    if (m_pDebugSettings) delete m_pDebugSettings;
    if (m_pETHSettings) delete m_pETHSettings;
    if (m_pI2CSettings) delete m_pI2CSettings;
    if (m_pFPGAsettings) delete m_pFPGAsettings;
    if (m_pSenseSettings) delete m_pSenseSettings;
    if (pAtmel) delete pAtmel;
    if (m_pAtmelWatcher) delete m_pAtmelWatcher;
    if (m_pStatusInterface) delete m_pStatusInterface;
    if (m_pSystemInterface) delete m_pSystemInterface;
    if (m_pSystemInfo) delete m_pSystemInfo;
    if (m_pAdjHandler) delete m_pAdjHandler;
}


void cCOM5003dServer::doConfiguration()
{
    QStringList args;
    args = app->arguments();
    if (args.count() != 2) // we want exactly 1 parameter
    {
        m_nerror = parameterError;
        emit abortInit();
    }
    else
    {
        if (myXMLConfigReader->loadSchema(defaultXSDFile))
        {
            // we want to initialize all settings first
            m_pDebugSettings = new cDebugSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(QString&)),m_pDebugSettings,SLOT(configXMLInfo(const QString&)));
            m_pETHSettings = new cETHSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(QString&)),m_pETHSettings,SLOT(configXMLInfo(const QString&)));
            m_pI2CSettings = new cI2CSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(QString&)),m_pI2CSettings,SLOT(configXMLInfo(const QString&)));
            m_pFPGAsettings = new cFPGASettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(QString&)),m_pFPGAsettings,SLOT(configXMLInfo(const QString&)));
            m_pSenseSettings = new cSenseSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(QString&)),m_pSenseSettings,SLOT(configXMLInfo(QString&)));
            m_pSourceSettings = new cSourceSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(QString&)),m_pSourceSettings,SLOT(configXMLInfo(QString&)));

            QString s = args.at(1);
            if (myXMLConfigReader->loadXML(s)) // the first parameter should be the filename
            {
                // xmlfile ok -> nothing to do .. the configreader will emit all configuration
                // signals and after this the finishedparsingXML signal
            }
            else
            {
                m_nerror = xmlfileError;
                emit abortInit();
            }
        }
        else
        {
            m_nerror = xsdfileError;
            emit abortInit();
        }
    }
}


void cCOM5003dServer::doWait4Atmel()
{
    pAtmel = new cATMEL(m_pI2CSettings->getDeviceNode(), m_pI2CSettings->getI2CAdress(i2cSettings::atmel), m_pDebugSettings->getDebugLevel());
    m_pAtmelWatcher = new cAtmelWatcher(m_pDebugSettings->getDebugLevel(), m_pFPGAsettings->getDeviceNode(), 10000, 100);


    m_nerror = atmelError; // we preset error
    connect(m_pAtmelWatcher,SIGNAL(timeout()),this,SIGNAL(abortInit()));
    connect(m_pAtmelWatcher,SIGNAL(running()),this,SIGNAL(atmelRunning()));
    m_pAtmelWatcher->start();
}


void cCOM5003dServer::doSetupServer()
{
    m_pSystemInfo = new cSystemInfo();
    m_pAdjHandler = new cAdjustment(m_pSystemInfo, m_pI2CSettings->getDeviceNode(), m_pDebugSettings->getDebugLevel(), m_pI2CSettings->getI2CAdress(i2cSettings::flash) );

    cPCBServer::setupServer(); // here our scpi interface gets instanciated, we need this for further steps

    scpiConnectionList.append(m_pStatusInterface = new cStatusInterface(m_pAdjHandler));
    scpiConnectionList.append(m_pSystemInterface = new cSystemInterface(this, m_pAdjHandler, m_pSystemInfo));
    scpiConnectionList.append(m_pSenseInterface = new cSenseInterface(m_pSenseSettings));
    scpiConnectionList.append(m_pSamplingInterface = new cSamplingInterface(m_pSamplingSettings));
    scpiConnectionList.append(m_pSourceInterface = new cSourceInterface(m_pSourceSettings));

    m_pAdjHandler->addAdjFlashObject(m_pSenseInterface); // we add the senseinterface to both
    m_pAdjHandler->addAdjXMLObject(m_pSenseInterface); // adjustment list (flash and xml)

    initSCPIConnections();

    myServer->startServer(m_pETHSettings->getPort(server)); // and can start the server now
}


void cCOM5003dServer::doCloseServer()
{
    app->exit(m_nerror);
}





