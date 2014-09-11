#include <scpicommand.h>

#include "senserange.h"
#include "scpidelegate.h"


cSenseRange::cSenseRange(QString name, QString alias, bool avail, double rValue, double rejection, double ovrejection, quint8 rselcode, quint8 rspec)
    :m_sName(name), m_sAlias(alias), m_bAvail(avail), m_fRValue(rValue), m_fRejection(rejection), m_fOVRejection(ovrejection), m_nSelCode(rselcode), m_nRSpec(rspec)
{
    m_pJustdata = new cCOM5003JustData();
}


cSenseRange::~cSenseRange()
{
    delete m_pJustdata;
}


void cSenseRange::initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"TYPE",SCPI::isQuery,scpiInterface, SenseRange::cmdType);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS",SCPI::isQuery,scpiInterface, SenseRange::cmdAlias);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"AVAIL",SCPI::isQuery,scpiInterface, SenseRange::cmdAvail);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"URVALUE",SCPI::isQuery,scpiInterface, SenseRange::cmdValue);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"REJECTION",SCPI::isQuery,scpiInterface, SenseRange::cmdRejection);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"OVREJECTION",SCPI::isQuery,scpiInterface, SenseRange::cmdOVRejection);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));

    m_pJustdata->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(m_sName), scpiInterface);
}


quint8 cSenseRange::getAdjustmentStatus()
{
    return m_pJustdata->getAdjustmentStatus();
}


QString &cSenseRange::getName()
{
    return m_sName;
}


double cSenseRange::getUrvalue()
{
    return m_fRValue;
}


quint8 cSenseRange::getSelCode()
{
    return m_nSelCode;
}


cCOM5003JustData *cSenseRange::getJustData()
{
    return m_pJustdata;
}


bool cSenseRange::getAvail()
{
    return m_bAvail;
}


void cSenseRange::setAvail(bool b)
{
    m_bAvail = b;
}


void cSenseRange::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case SenseRange::cmdType:
        sOutput = m_ReadRangeType(sInput);
        break;
    case SenseRange::cmdAlias:
        sOutput = m_ReadRangeAlias(sInput);
        break;
    case SenseRange::cmdAvail:
        sOutput = m_ReadRangeAvail(sInput);
        break;
    case SenseRange::cmdValue:
        sOutput = m_ReadRangeValue(sInput);
        break;
    case SenseRange::cmdRejection:
        sOutput = m_ReadRangeRejection(sInput);
        break;
    case SenseRange::cmdOVRejection:
        sOutput = m_ReadRangeOVRejection(sInput);
        break;
    }
}


QString cSenseRange::m_ReadRangeType(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nRSpec); // phs. or virt.
    else
        return SCPI::scpiAnswer[SCPI::nak];

}


QString cSenseRange::m_ReadRangeAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseRange::m_ReadRangeAvail(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_bAvail)
            return "1";
        else
            return "0";
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseRange::m_ReadRangeValue(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fRValue);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseRange::m_ReadRangeRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fRejection);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseRange::m_ReadRangeOVRejection(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fOVRejection);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

