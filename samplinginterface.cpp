#include <QString>
#include <QList>
#include <scpi.h>

#include "scpiconnection.h"
#include "samplerange.h"
#include "samplingsettings.h"
#include "samplinginterface.h"
#include "atmel.h"
#include "com5003d.h"

extern cATMEL* pAtmel;

cSamplingInterface::cSamplingInterface(cCOM5003dServer *server, cSamplingSettings *samplingSettings)
{
    m_pMyServer = server;

    m_pllChannelList.append("0");
    m_pllChannelList.append("m0");
    m_pllChannelList.append("m1");
    m_pllChannelList.append("m2");
    m_pllChannelList.append("m3");
    m_pllChannelList.append("m4");
    m_pllChannelList.append("m5");

    QList<SamplingSystem::cChannelSettings*> mySettings;

    mySettings = samplingSettings->getChannelSettings();
    m_sName = "s0";
    m_sDescription = "Samplingsytem base frequency 10Hz..400Hz";
    m_sAlias = mySettings.at(0)->m_sAlias;
    m_bAvail = mySettings.at(0)->m_bAvail;
    m_sVersion = SamplingSystem::Version;
    m_nType = 0;

    m_SampleRangeList.append(new cSampleRange("F50Hz", 504, 0));
    m_SampleRangeList.append(new cSampleRange("F20Hz", 720, 1));

    pAtmel->setSamplingRange(0); // default we set 50Hz 504 samples
    setNotifierSampleChannelRange(); // we must intialize our setting (notifier)
}


void cSamplingInterface::initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1SAMPLE").arg(leadingNodes),"VERSION", SCPI::isQuery, scpiInterface, SamplingSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE").arg(leadingNodes),"SRATE", SCPI::isQuery, scpiInterface, SamplingSystem::cmdSampleRate);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelAlias);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"TYPE", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelType);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"RANGE", SCPI::isQuery | SCPI::isCmdwP , scpiInterface, SamplingSystem::cmdChannelRange);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2:RANGE").arg(leadingNodes).arg(m_sName),"CATALOG", SCPI::isQuery, scpiInterface, SamplingSystem::cmdChannelRangeCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"PLL", SCPI::isQuery | SCPI::isCmdwP , scpiInterface, SamplingSystem::cmdPLL);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2:PLL").arg(leadingNodes).arg(m_sName),"CATALOG", SCPI::isQuery, scpiInterface, SamplingSystem::cmdPLLCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));

    for (int i = 0; i < m_SampleRangeList.count(); i++)
        m_SampleRangeList.at(i)->initSCPIConnection(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName), scpiInterface);

}


void cSamplingInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SAMPLE;%1;1;%2;%3;").arg(m_sName).arg(m_sDescription).arg(port));
}


void cSamplingInterface::unregisterResource(cRMConnection *rmConnection)
{
    unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SAMPLE;%1;").arg(m_sName));
}


void cSamplingInterface::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case SamplingSystem::cmdVersion:
        sOutput = m_ReadVersion(sInput);
        break;
    case SamplingSystem::cmdSampleRate:
        sOutput = m_ReadSampleRate(sInput);
        break;
    case SamplingSystem::cmdChannelCat:
        sOutput = m_ReadSamplingChannelCatalog(sInput);
        break;
    case SamplingSystem::cmdChannelAlias:
        sOutput = m_ReadAlias(sInput);
        break;
    case SamplingSystem::cmdChannelType:
        sOutput = m_ReadType(sInput);
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
    case SamplingSystem::cmdPLL:
        sOutput = m_ReadWritePLL(sInput);
        break;
    case SamplingSystem::cmdPLLCat:
        sOutput = m_ReadPLLCatalog(sInput);
        break;

    }
}


QString cSamplingInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSamplingInterface::m_ReadSampleRate(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        int i;
        QString s;
        s = notifierSampleChannelRange.getString(); // our actual sample channels range
        for  (i = 0; i < m_SampleRangeList.count(); i++)
            if (m_SampleRangeList.at(i)->getName() == s)
                break;
        return QString("%1").arg(m_SampleRangeList.at(i)->getSRate());
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSamplingInterface::m_ReadSamplingChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sName; // we only have 1 channel
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSamplingInterface::m_ReadAlias(QString &sInput)
{
     cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_sAlias);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSamplingInterface::m_ReadType(QString &sInput)
{
     cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nType);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSamplingInterface::m_ReadStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        quint32 r;
        r = (m_bAvail) ? 0 : 2 << 31;
        return QString("%1").arg(r);
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}



QString cSamplingInterface::m_ReadWriteSamplingRange(QString &sInput)
{
    int i;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        emit notifier(&notifierSampleChannelRange); // we need this in case that the client wants notification
        return notifierSampleChannelRange.getString();
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
                {
                    setNotifierSampleChannelRange();
                    return SCPI::scpiAnswer[SCPI::ack];
                }
                else
                    return SCPI::scpiAnswer[SCPI::errexec];
            else
                return SCPI::scpiAnswer[SCPI::nak];

        }
        else
            return SCPI::scpiAnswer[SCPI::nak];
    }
}


QString cSamplingInterface::m_ReadSamplingRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        int i;
        QString s;
        for (i = 0; i < m_SampleRangeList.count(); i++)
            s += m_SampleRangeList.at(i)->getName() + ";";
        s += m_SampleRangeList.at(i)->getName();

        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSamplingInterface::m_ReadWritePLL(QString &sInput)
{
    quint8 pll;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (pAtmel->readPLLChannel(pll) == cmddone)
        {
            if (pll < 7) // then everything is ok
                return m_pllChannelList.at(pll);
        }

        return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
    {
        if (cmd.isCommand(1))
        {
            QString pllchn = cmd.getParam(0);
            pll = m_pllChannelList.indexOf(pllchn);

            if (pAtmel->setPLLChannel(pll) == cmddone)
                return SCPI::scpiAnswer[SCPI::ack];
            else
                return SCPI::scpiAnswer[SCPI::errexec];
        }

        return SCPI::scpiAnswer[SCPI::nak];
    }
}


QString cSamplingInterface::m_ReadPLLCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        int i;
        QString s;
        for (i = 1; i < m_pllChannelList.count()-1; i++)
            s += m_pllChannelList.at(i) + ";";
        s += m_pllChannelList.at(i);

        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


void cSamplingInterface::setNotifierSampleChannelRange()
{
    int i;
    quint8 sRange;

    if (pAtmel->readSamplingRange(sRange) == cmddone)
    {
        for (i = 0; i < m_SampleRangeList.count(); i++)
            if (m_SampleRangeList.at(i)->getSelCode() == sRange)
                break;
        notifierSampleChannelRange = m_SampleRangeList.at(i)->getName();
    }
    else
        notifierSampleChannelRange = m_SampleRangeList.at(0)->getName();
}





