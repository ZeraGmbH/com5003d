#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QStringList>
#include <QDebug>
#include <xmlconfigreader.h>
#include <QCoreApplication>
#include <zeraserver.h>


#include "com5003dglobal.h"
#include "com5003d.h"
#include "pcbserver.h"
#include "debugsettings.h"
#include "ethsettings.h"
#include "i2csettings.h"
#include "fpgasettings.h"
#include "sensesettings.h"
#include "samplingsettings.h"
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
#include "rmconnection.h"


cATMEL* pAtmel; // we take a static object for atmel connection

cCOM5003dServer::cCOM5003dServer(QObject *parent)
    :cPCBServer(parent)
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

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    stateCONF->addTransition(this, SIGNAL(abortInit()),stateFINISH); // from anywhere we arrive here if some error

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* statewait4Atmel = new QState(stateCONF); // we snchronize on atmel running
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    stateSendRMIdentandRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(myXMLConfigReader, SIGNAL(finishedParsingXML()), statewait4Atmel);
    statewait4Atmel->addTransition(this, SIGNAL(atmelRunning()), statesetupServer);
    statesetupServer->addTransition(this, SIGNAL(serverSetup()), stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, SIGNAL(entered()), this, SLOT(doConfiguration()));
    QObject::connect(statewait4Atmel, SIGNAL(entered()), this, SLOT(doWait4Atmel()));
    QObject::connect(statesetupServer, SIGNAL(entered()), this, SLOT(doSetupServer()));
    QObject::connect(stateconnect2RM, SIGNAL(entered()), this, SLOT(doConnect2RM()));
    QObject::connect(stateSendRMIdentandRegister, SIGNAL(entered()), this, SLOT(doIdentAndRegister()));
    QObject::connect(stateFINISH, SIGNAL(entered()), this, SLOT(doCloseServer()));

    m_pInitializationMachine->start();
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
    if (m_pRMConnection) delete m_pRMConnection;
}


void cCOM5003dServer::doConfiguration()
{
    QStringList args;
    args = QCoreApplication::instance()->arguments();
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
            connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pDebugSettings,SLOT(configXMLInfo(const QString&)));
            m_pETHSettings = new cETHSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pETHSettings,SLOT(configXMLInfo(const QString&)));
            m_pI2CSettings = new cI2CSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pI2CSettings,SLOT(configXMLInfo(const QString&)));
            m_pFPGAsettings = new cFPGASettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pFPGAsettings,SLOT(configXMLInfo(const QString&)));
            m_pSenseSettings = new cSenseSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pSenseSettings,SLOT(configXMLInfo(const QString&)));
            m_pSourceSettings = new cSourceSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pSourceSettings,SLOT(configXMLInfo(const QString&)));
            m_pSamplingSettings = new cSamplingSettings(myXMLConfigReader);
            connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pSamplingSettings,SLOT(configXMLInfo(const QString&)));

            QString s = args.at(1);
            qDebug() << s;
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

    resourceList.append(m_pSenseInterface); // all our resources
    resourceList.append(m_pSamplingInterface);
    resourceList.append(m_pSourceInterface);

    m_pAdjHandler->addAdjFlashObject(m_pSenseInterface); // we add the senseinterface to both
    m_pAdjHandler->addAdjXMLObject(m_pSenseInterface); // adjustment list (flash and xml)
    m_pAdjHandler->importJDataFlash(); // we read adjustmentdata at least once

    initSCPIConnections();

    myServer->startServer(m_pETHSettings->getPort(server)); // and can start the server now

    // our resource mananager connection must be opened after configuration is done
    m_pRMConnection = new cRMConnection(m_pETHSettings->getRMIPadr(), m_pETHSettings->getPort(resourcemanager), m_pDebugSettings->getDebugLevel());
    connect(m_pRMConnection, SIGNAL(connectionRMError()), this, SIGNAL(abortInit()));
    connect(m_pRMConnection , SIGNAL(communicationError()), this, SIGNAL(abortInit()));
    // so we must complete our state machine here
    stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), stateSendRMIdentandRegister);


    emit serverSetup(); // so we enter state machine's next state
}


void cCOM5003dServer::doCloseServer()
{
    QCoreApplication::instance()->exit(m_nerror);
}


void cCOM5003dServer::doConnect2RM()
{
    m_pRMConnection->connect2RM();
}


void cCOM5003dServer::doIdentAndRegister()
{
    m_pRMConnection->SendIdent(getName());

    qDebug() << resourceList.count();
    for (int i = 0; i << resourceList.count(); i++)
        resourceList.at(i)->registerResource(m_pRMConnection);
}







