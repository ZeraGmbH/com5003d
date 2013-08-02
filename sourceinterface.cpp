#include <QList>
#include <scpi.h>

#include "xmlsettings.h"
#include "scpiconnection.h"
#include "resource.h"

#include "sourcesettings.h"
#include "sourceinterface.h"
#include "fpzchannel.h"


cSourceInterface::cSourceInterface(cSourceSettings *sourceSettings)
{
    QList<SourceSystem::cChannelSettings*> mySettings;

    mySettings = sourceSettings->getChannelSettings();

    // we have 4 frequency output channels
    cFPZChannel* pChannel;
    pChannel = new cFPZChannel("Reference frequency output 0..1MHz", 0, mySettings.at(0) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel("Reference frequency output 0..1MHz", 1, mySettings.at(1) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel("Reference frequency output 0..1MHz", 2,  mySettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel("Reference frequency output 0..1MHz", 3, mySettings.at(3) );
    m_ChannelList.append(pChannel);

    m_sVersion = SourceSystem::Version;
}


cSourceInterface::~cSourceInterface()
{
    cFPZChannel* cptr;
    for ( int i = 0; i < m_ChannelList.count(); i++)
    {
        cptr = m_ChannelList.at(i);
        delete cptr;
    }
}


void cSourceInterface::initSCPIConnection(QString leadingNodes, cSCPI* scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1SOURCE").arg(leadingNodes),"VERSION",SCPI::isQuery,scpiInterface, SourceSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SOURCE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, scpiInterface, SourceSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));

    for (int i = 0; i < m_ChannelList.count(); i++)
        m_ChannelList.at(i)->initSCPIConnection(QString("%1SOURCE").arg(leadingNodes),scpiInterface);
}


void cSourceInterface::registerResource(QDataStream &stream)
{
}


void cSourceInterface::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case SourceSystem::cmdVersion:
        sOutput = m_ReadVersion(sInput);
        break;
    case SourceSystem::cmdChannelCat:
        sOutput = m_ReadSourceChannelCatalog(sInput);
        break;
    }
}


QString cSourceInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion+";";
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSourceInterface::m_ReadSourceChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {   QString s;
        for (int i = 0; i < m_ChannelList.count(); i++ )
            s += QString("fo%1;").arg(i);

        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

/*
void cSenseInterface::registerResource(QDataStream &stream)
{
    for (int i = 0; i < m_ChannelList.count(); i++)
        if (m_ChannelList.at(i)->isAvail() )
            stream << QString("RESOURCE:ADD SENSE;%1;;%2").arg(m_ChannelList.at(i)->getName()
                                                      .arg((m_ChannelList.at(i)->getDescription());
}
*/



