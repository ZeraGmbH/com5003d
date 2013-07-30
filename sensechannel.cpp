#include <QList>
#include <QString>

#include <scpi.h>
#include <scpicommand.h>
#include "atmel.h"
#include "senserange.h"
#include "scpiconnection.h"
#include "sensesettings.h"
#include "sensechannel.h"

extern cATMEL* pAtmel;


cSenseChannel::cSenseChannel(QString description, QString unit, SenseSystem::cChannelSettings *cSettings, quint8 nr)
    :m_sDescription(description), m_sUnit(unit)
{
    m_sName = QString("m%1").arg(nr);
    m_sIdent = cSettings->m_sName;
    m_nCtrlChannel = cSettings->m_nCtrlChannel;
    m_nDspChannel = cSettings->m_nDspChannel;
    m_bAvail = cSettings->avail;
}



cSenseChannel::~cSenseChannel()
{
    for (int i = 0; i < m_RangeList.count(); i++)
        delete m_RangeList.at(i);
}


void cSenseChannel::initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"IDENT", SCPI::isQuery, scpiInterface, SenseChannel::cmdIdent);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"Type", SCPI::isQuery, scpiInterface, SenseChannel::cmdType);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"UNIT", SCPI::isQuery, scpiInterface, SenseChannel::cmdUnit);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"DSPCHANNEL", SCPI::isQuery, scpiInterface, SenseChannel::cmdDspChannel);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, scpiInterface, SenseChannel::cmdStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"RANGE", SCPI::isQuery, scpiInterface, SenseChannel::cmdRange);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2:RANGE").arg(leadingNodes).arg(m_sName),"CATALOG", SCPI::isQuery, scpiInterface, SenseChannel::cmdRangeCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));

    for (int i = 0;i < m_RangeList.count(); i++)
        m_RangeList.at(i)->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(m_sName), scpiInterface );
}


void cSenseChannel::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case SenseChannel::cmdIdent:
        sOutput = m_ReadIdent(sInput);
        break;
    case SenseChannel::cmdType:
        sOutput = m_ReadType(sInput);
        break;
    case SenseChannel::cmdUnit:
        sOutput = m_ReadUnit(sInput);
        break;
    case SenseChannel::cmdDspChannel:
        sOutput = m_ReadDspChannel(sInput);
        break;
    case SenseChannel::cmdStatus:
        sOutput = m_ReadChannelStatus(sInput);
        break;
    case SenseChannel::cmdRange:
        sOutput = m_ReadWriteRange(sInput);
        break;
    case SenseChannel::cmdRangeCat:
        sOutput = m_ReadRangeCatalog(sInput);
        break;
    }

}


void cSenseChannel::setRangeList(QList<cSenseRange*> &list)
{
    m_RangeList = list;
}


QList<cSenseRange *> &cSenseChannel::getRangeList()
{
    return m_RangeList;
}


cSenseRange *cSenseChannel::getRange(QString &name)
{
    int i;
    for (i = 0; i < m_RangeList.count(); i++)
        if (m_RangeList.at(i)->getName() == name)
            break;

    if (i < m_RangeList.count())
        return m_RangeList.at(i);
    else
        return 0;
}


quint8 cSenseChannel::getAdjustmentStatus()
{
    quint8 adj = 255;
    for (int i = 0; i < m_RangeList.count(); i++ )
        adj &= m_RangeList.at(i)->getAdjustmentStatus();
    return adj;
}


QString &cSenseChannel::getName()
{
    return m_sName;
}


QString &cSenseChannel::getDescription()
{
    return m_sDescription;
}


bool cSenseChannel::isAvail()
{
    return m_bAvail;
}


QString cSenseChannel::m_ReadIdent(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sIdent;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseChannel::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("0");
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseChannel::m_ReadUnit(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sUnit;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseChannel::m_ReadDspChannel(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nDspChannel);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseChannel::m_ReadChannelStatus(QString &sInput)
{
    quint8 status;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if ( pAtmel->readChannelStatus(m_nCtrlChannel, status) == cmddone )
        {
            quint32 r;
            r = ((m_bAvail) ? 0 : 2 << 31) || status;
            return QString("%1").arg(r);
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseChannel::m_ReadWriteRange(QString &sInput)
{
    int i;
    quint8 range;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if ( pAtmel->readRange(m_nCtrlChannel, range) == cmddone )
        {
            for (i = 0; i < m_RangeList.count(); i++)
                if (m_RangeList.at(i)->getSelCode() == range)
                    break;
            return m_RangeList.at(i)->getName();
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
        if (cmd.isCommand(1))
        {
            QString rng = cmd.getParam(1);
            for  (i = 0; i < m_RangeList.count(); i++)
                if (m_RangeList.at(i)->getName() == rng)
                    break;
            if ( pAtmel->setRange(m_nCtrlChannel, m_RangeList.at(i)->getSelCode()) == cmddone)
                return SCPI::scpiAnswer[SCPI::ack];
            else
                return SCPI::scpiAnswer[SCPI::errexec];

        }

    return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseChannel::m_ReadRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s;
        for (int i = 0; i < m_RangeList.count(); i++)
            s += (m_RangeList.at(i)->getName() + ";");

        return s; // phs. or virt.
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}
