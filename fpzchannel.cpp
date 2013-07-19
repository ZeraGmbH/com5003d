#include <QList>
#include <QString>

#include <scpi.h>
#include <scpicommand.h>
#include "scpiconnection.h"
#include "sourcesettings.h"
#include "fpzchannel.h"


cFPZChannel::cFPZChannel(QString description, SourceSystem::cChannelSettings *cSettings)
    :m_sDescription(description)
{
    m_sName = cSettings->m_sName;
    m_sDspServer = cSettings->m_sDspServer;
    m_nDspChannel = cSettings->m_nDspChannel;
    m_nType = 0;
    m_fFormFactor = FPZChannel::FormFactor;
    m_bAvail = cSettings->avail;
}


void cFPZChannel::initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"Type", SCPI::isQuery, scpiInterface, FPZChannel::cmdType);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"DSPSERVER", SCPI::isQuery, scpiInterface, FPZChannel::cmdDspServer);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"DSPCHANNEL", SCPI::isQuery, scpiInterface, FPZChannel::cmdDspChannel);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, scpiInterface, FPZChannel::cmdStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"FFACTOR", SCPI::isQuery, scpiInterface, FPZChannel::cmdFormFactor);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
}


void cFPZChannel::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case FPZChannel::cmdType:
        sOutput = m_ReadType(sInput);
        break;
    case FPZChannel::cmdDspServer:
        sOutput = m_ReadDspServer(sInput);
        break;
    case FPZChannel::cmdDspChannel:
        sOutput = m_ReadDspChannel(sInput);
        break;
    case FPZChannel::cmdStatus:
        sOutput = m_ReadChannelStatus(sInput);
        break;
    case FPZChannel::cmdFormFactor:
        sOutput = m_ReadFFactor(sInput);
        break;
    }
}



QString &cFPZChannel::getName()
{
    return m_sName;
}


QString &cFPZChannel::getDescription()
{
    return m_sDescription;
}


bool cFPZChannel::isAvail()
{
    return m_bAvail;
}


QString cFPZChannel::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nType);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadDspServer(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sDspServer;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadDspChannel(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nDspChannel);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        quint32 r;
        r = ((m_bAvail) ? 0 : 2 << 31);
            return QString("%1").arg(r);
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadFFactor(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fFormFactor);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}
