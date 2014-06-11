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
    m_sAlias[0] = cSettings->m_sAlias[0];
    m_sAlias[1] = cSettings->m_sAlias[1];
    m_nCtrlChannel = cSettings->m_nCtrlChannel;
    m_nDspChannel = cSettings->m_nDspChannel;
    m_nOverloadBit = cSettings->m_nOverloadBit;
    m_bAvail = cSettings->avail;
    m_nMMode = SenseChannel::modeAC; // the default
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

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, scpiInterface, SenseChannel::cmdAlias);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"TYPE", SCPI::isQuery, scpiInterface, SenseChannel::cmdType);
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
    delegate = new cSCPIDelegate(QString("%1%2:STATUS").arg(leadingNodes).arg(m_sName),"RESET", SCPI::isCmd, scpiInterface, SenseChannel::cmdStatusReset);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"RANGE", SCPI::isQuery | SCPI::isCmdwP, scpiInterface, SenseChannel::cmdRange);
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
    case SenseChannel::cmdAlias:
        sOutput = m_ReadAlias(sInput);
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
    case SenseChannel::cmdStatusReset:
        sOutput = m_StatusReset(sInput);
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
    setNotifierSenseChannelRangeCat();
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


QString &cSenseChannel::getAlias()
{
    if (m_nMMode == 0)
        return m_sAlias[0];
    else
        return m_sAlias[1];
}


QString &cSenseChannel::getDescription()
{
    return m_sDescription;
}


void cSenseChannel::setDescription(const QString &s)
{
    m_sDescription = s;
}


void cSenseChannel::setUnit(QString &s)
{
    m_sUnit = s;
}


void cSenseChannel::setMMode(int m)
{
    m_nMMode = m;
}


bool cSenseChannel::isAvail()
{
    return m_bAvail;
}


QString cSenseChannel::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_nMMode == 0)
            return m_sAlias[0];
        else
            return m_sAlias[1];
    }
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
    quint16 status;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if ( pAtmel->readCriticalStatus(status) == cmddone )
        {
            quint32 r;
            r = ((m_bAvail) ? 0 : 1 << 31);
            if ( (m_nOverloadBit >= 0)  && (status & (1 << m_nOverloadBit) ) > 0)
                r |= 1;
            return QString("%1").arg(r);
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseChannel::m_StatusReset(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        if (m_nOverloadBit >= 0)
        {
            if ( pAtmel->resetCriticalStatus((quint16)(1 << m_nOverloadBit)) == cmddone )
                return SCPI::scpiAnswer[SCPI::ack];
            else
                return SCPI::scpiAnswer[SCPI::errexec];
        }
        else
            return SCPI::scpiAnswer[SCPI::ack];
    }

    return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseChannel::m_ReadWriteRange(QString &sInput)
{
    int i;
    quint8 range, mode;
    cSCPICommand cmd = sInput;

    if ( pAtmel->readMeasMode(mode) == cmddone )
    {
        if (cmd.isQuery())
        {
            if (mode == SenseChannel::modeAC) // wir sind im normalberieb
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
            {
                if (mode == 1)
                    return "R0V";
                else
                    return "R10V";
            }
        }

        else
        {
            if (cmd.isCommand(1))
            {
                QString rng = cmd.getParam(0);
                int anz = m_RangeList.count();
                for  (i = 0; i < anz; i++)
                    if (m_RangeList.at(i)->getName() == rng)
                        break;
                if ( (i < anz) && (m_RangeList.at(i)->getAvail()) )
                {
                    // we know this range and it's available
                    if (m_nMMode == SenseChannel::modeAC)
                    {
                        if ( pAtmel->setRange(m_nCtrlChannel, m_RangeList.at(i)->getSelCode()) == cmddone)
                            return SCPI::scpiAnswer[SCPI::ack];
                        else
                            return SCPI::scpiAnswer[SCPI::errexec];
                    }
                    else
                    {
                        if (m_RangeList.at(i)->getName() == "R0V")
                            pAtmel->setMeasMode(1);
                        else
                            pAtmel->setMeasMode(2);

                        return SCPI::scpiAnswer[SCPI::ack];
                    }

                }

            }

            return SCPI::scpiAnswer[SCPI::nak];
        }
    }

    else
        return SCPI::scpiAnswer[SCPI::errexec];

}


QString cSenseChannel::m_ReadRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {

        return notifierSenseChannelRangeCat.getString();
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


void cSenseChannel::setNotifierSenseChannelRangeCat()
{
    int i;
    QString s;
    for (i = 0; i < m_RangeList.count()-1; i++)
        s += (m_RangeList.at(i)->getName() + ";");
    s += m_RangeList.at(i)->getName();

    notifierSenseChannelRangeCat = s; // phs. or virt.
}
