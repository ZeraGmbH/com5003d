#include <QString>
#include <QList>
#include <scpi.h>

#include "scpiconnection.h"
#include "samplerange.h"
#include "samplingsettings.h"
#include "samplinginterface.h"
#include "atmel.h"

extern cATMEL* pAtmel;

cSamplingInterface::cSamplingInterface(cSamplingSettings *samplingSettings)
{
    QList<SamplingSystem::cChannelSettings*> mySettings;

    mySettings = samplingSettings->getChannelSettings();
    m_sName = "s0";
    m_sIdent = mySettings.at(0)->m_sIdent;
    m_bAvail = mySettings.at(0)->m_bAvail;
    m_sVersion = SamplingSystem::Version;
    m_nType = 0;

    m_SampleRangeList.append(new cSampleRange("F20Hz", 720, 0));
    m_SampleRangeList.append(new cSampleRange("F50Hz", 504, 1));
}


void cSamplingInterface::initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1SAMPLE").arg(leadingNodes),"VERSION", SCPI::isQuery, scpiInterface, SamplingSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"IDENT", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelIdent);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"TYPE", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelType);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"MODE", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelMode);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"RANGE", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelRange);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2:RANGE").arg(leadingNodes).arg(m_sName),"CATALOG", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelRangeCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));

    for (int i = 0; i < m_SampleRangeList.count(); i++)
        m_SampleRangeList.at(i)->initSCPIConnection(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName), scpiInterface);

}


void cSamplingInterface::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case SamplingSystem::cmdVersion:
        sOutput = m_ReadVersion(sInput);
        break;
    case SamplingSystem::cmdChannelCat:
        sOutput = m_ReadSamplingChannelCatalog(sInput);
        break;
    case SamplingSystem::cmdChannelIdent:
        sOutput = m_ReadIdent(sInput);
        break;
    case SamplingSystem::cmdChannelType:
        sOutput = m_ReadType(sInput);
        break;
    case SamplingSystem::cmdChannelMode:
        sOutput = m_ReadWriteMode(sInput);
        break;
    case SamplingSystem::cmdChannelStatus:
        sOutput = m_ReadStatus(sInput);
        break;
    case SamplingSystem::cmdChannelRange:
        sOutput = m_ReadWriteSamplingRange(sInput);
        break;
    case SamplingSystem::cmdChannelRangeCat:
        sOutput = m_ReadSamplingRangeCatalog(sInput);
        break;
    }
}


QString cSamplingInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion+";";
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}


QString cSamplingInterface::m_ReadSamplingChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sName+";"; // we only have 1 channel
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}


QString cSamplingInterface::m_ReadIdent(QString &sInput)
{
     cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1;").arg(m_sIdent);
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}


QString cSamplingInterface::m_ReadType(QString &sInput)
{
     cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1;").arg(m_nType);
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}


QString cSamplingInterface::m_ReadWriteMode(QString &sInput)
{
    quint8 sMode;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (pAtmel->readSamplingMode(sMode) == cmddone)
        {
            return QString("%1;").arg(sMode);
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec]+";";
    }
    else
    {
        if (cmd.isCommand(1))
        {
            bool ok;
            QString smode = cmd.getParam(0);
            quint8 mode = smode.toInt(&ok);

            if (ok && ((mode==0) || (mode==1)) )
            {
                if (pAtmel->setSamplingMode(mode) == cmddone)
                    return SCPI::scpiAnswer[SCPI::ack]+";";
                else
                    return SCPI::scpiAnswer[SCPI::errexec]+";";
            }
            else
                return SCPI::scpiAnswer[SCPI::nak]+";";
        }
        else
            return SCPI::scpiAnswer[SCPI::nak]+";";
    }
}


QString cSamplingInterface::m_ReadStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        quint32 r;
        r = (m_bAvail) ? 0 : 2 << 31;
        return QString("%1;").arg(r);
    }
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}



QString cSamplingInterface::m_ReadWriteSamplingRange(QString &sInput)
{
    int i;
    quint8 sRange;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (pAtmel->readSamplingRange(sRange) == cmddone)
        {
            for (i = 0; i < m_SampleRangeList.count(); i++)
                if (m_SampleRangeList.at(i)->getSelCode() == sRange)
                    break;
            return m_SampleRangeList.at(i)->getName()+";";
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec]+";";
    }
    else
    {
        if (cmd.isCommand(1))
        {
            QString srng = cmd.getParam(0);
            for  (i = 0; i < m_SampleRangeList.count(); i++)
                if (m_SampleRangeList.at(i)->getName() == srng)
                    break;
            if (i < m_SampleRangeList.count())
                if ( pAtmel->setSamplingRange(m_SampleRangeList.at(i)->getSelCode()) == cmddone)
                    return SCPI::scpiAnswer[SCPI::ack]+";";
                else
                    return SCPI::scpiAnswer[SCPI::errexec]+";";
            else
                return SCPI::scpiAnswer[SCPI::nak]+";";

        }
        else
            return SCPI::scpiAnswer[SCPI::nak]+";";
    }
}


QString cSamplingInterface::m_ReadSamplingRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s;
        for (int i = 0; i < m_SampleRangeList.count(); i++)
            s += m_SampleRangeList.at(i)->getName() + ";";

        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}



