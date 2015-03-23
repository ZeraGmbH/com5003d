#include <QList>
#include <scpi.h>

#include "xmlsettings.h"
#include "scpiconnection.h"
#include "resource.h"
#include "com5003d.h"
#include "frqinputsettings.h"
#include "frqinputinterface.h"
#include "fpzinchannel.h"
#include "protonetcommand.h"


cFRQInputInterface::cFRQInputInterface(cCOM5003dServer *server, cFRQInputSettings* fpzinSettings)
{
    m_pMyServer = server;
    QList<FRQInputSystem::cChannelSettings*> mySettings;

    mySettings = fpzinSettings->getChannelSettings();

    // we have 4 frequency input channels
    cFPZInChannel* pChannel;
    pChannel = new cFPZInChannel("Frequency input 0..1MHz", 0, mySettings.at(0) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZInChannel("Frequency output 0..1MHz", 1, mySettings.at(1) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZInChannel("Frequency output 0..1MHz", 2,  mySettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZInChannel("Frequency output 0..1MHz", 3, mySettings.at(3) );
    m_ChannelList.append(pChannel);

    m_sVersion = FRQInputSystem::Version;
}


cFRQInputInterface::~cFRQInputInterface()
{
    cFPZInChannel* cptr;
    for ( int i = 0; i < m_ChannelList.count(); i++)
    {
        cptr = m_ChannelList.at(i);
        delete cptr;
    }
}


void cFRQInputInterface::initSCPIConnection(QString leadingNodes, cSCPI* scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1FRQINPUT").arg(leadingNodes),"VERSION",SCPI::isQuery,scpiInterface, FRQInputSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1FRQINPUT:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, scpiInterface, FRQInputSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        connect(m_ChannelList.at(i), SIGNAL(notifier(cNotificationString*)), this, SIGNAL(notifier(cNotificationString*)));
        connect(m_ChannelList.at(i), SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
        m_ChannelList.at(i)->initSCPIConnection(QString("%1FRQINPUT").arg(leadingNodes),scpiInterface);
    }
}


void cFRQInputInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    cFPZInChannel* pChannel;
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        pChannel = m_ChannelList.at(i);
        register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("FRQINPUT;%1;1;%2;%3;").arg(pChannel->getName()).arg(pChannel->getDescription()).arg(port));
    }
}


void cFRQInputInterface::unregisterResource(cRMConnection *rmConnection)
{
    cFPZInChannel* pChannel;
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        pChannel = m_ChannelList.at(i);
        unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("FRQINPUT;%1;").arg(pChannel->getName()));
    }
}


void cFRQInputInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case FRQInputSystem::cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        break;
    case FRQInputSystem::cmdChannelCat:
        protoCmd->m_sOutput = m_ReadChannelCatalog(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString cFRQInputInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFRQInputInterface::m_ReadChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        int i;
        QString s;
        for (i = 0; i < m_ChannelList.count()-1; i++ )
            s += m_ChannelList.at(i)->getName() + ";";
        s += m_ChannelList.at(i)->getName();

        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}





