// implemention cCOM5003JustData

#include <qdatastream.h>
#include <scpi.h>

#include "atmel.h"
#include "justdata.h"
#include "scpidelegate.h"
#include "com5003justdata.h"

extern cATMEL* pAtmel;

cCOM5003JustData::cCOM5003JustData()
{
    m_pGainCorrection = new cJustData(GainCorrOrder, 1.0);
    m_pPhaseCorrection = new cJustData(PhaseCorrOrder, 0.0);
    m_pOffsetCorrection =  new cJustData(OffsetCorrOrder, 0.0);
}


cCOM5003JustData::~cCOM5003JustData()
{
    delete m_pGainCorrection; 
    delete m_pPhaseCorrection;
    delete m_pOffsetCorrection;

    cSCPIDelegate* ptr;
    for (int i = 0; i < m_DelegateList.count(); i++)
    {
        ptr = m_DelegateList.at(i);
        delete ptr;
    }
}


void cCOM5003JustData::initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "GAIN", SCPI::CmdwP , scpiInterface, com5003JustGain);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "PHASE", SCPI::isCmdwP, scpiInterface, com5003JustPhase);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "OFFSET", SCPI::isCmdwP, scpiInterface, com5003JustOffset);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "STATUS", SCPI::isQuery, scpiInterface, com5003JustStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "COMPUTE", SCPI::isCmdwP || SCPI::isQuery, scpiInterface, com5003JustCompute);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));

    m_pGainCorrection->initSCPIConnection(QString("%1CORRECTION:GAIN").arg(leadingNodes), scpiInterface);
    m_pPhaseCorrection->initSCPIConnection(QString("%1CORRECTION:PHASE").arg(leadingNodes), scpiInterface);
    m_pOffsetCorrection->initSCPIConnection(QString("%1CORRECTION:OFFSET").arg(leadingNodes), scpiInterface);
}
	    

void cCOM5003JustData::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case com5003JustGain:
        sOutput = mReadGainCorrection(sInput);
        break;
    case com5003JustPhase:
        sOutput = mReadPhaseCorrection(sInput);
        break;
    case com5003JustOffset:
        sOutput = mReadOffsetCorrection(sInput);
        break;
    case com5003JustStatus:
        sOutput = m_ReadStatus(sInput);
        break;
    case com5003JustCompute:
        sOutput = m_ComputeJustData(sInput);
        break;
    }
}


QString cCOM5003JustData::mReadGainCorrection(QString& sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1;").arg(m_pGainCorrection->getCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval]+";";
    }
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}


QString cCOM5003JustData::mReadPhaseCorrection(QString& sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1;").arg(m_pPhaseCorrection->getCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval]+";";
    }
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";

}


QString cCOM5003JustData::mReadOffsetCorrection(QString& sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1;").arg(m_pOffsetCorrection->getCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval]+";";
    }
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";

}


QString cCOM5003JustData::m_ReadStatus(QString& sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        return QString("%1;").arg(getAdjustmentStatus());
    }
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}


QString cCOM5003JustData::m_ComputeJustData(QString& sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(0))
    {
        bool enable;
        if (pAtmel->getEEPROMAccessEnable(enable) == cmddone)
        {
            if (enable)
            {
                m_pGainCorrection->cmpCoefficients();
                m_pPhaseCorrection->cmpCoefficients();
                m_pOffsetCorrection->cmpCoefficients();
                return SCPI::scpiAnswer[SCPI::ack]+";";
            }
            else
                return SCPI::scpiAnswer[SCPI::erraut]+";";
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec]+";";
    }
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}


void cCOM5003JustData::Serialize(QDataStream& qds)  // zum schreiben aller justagedaten in flashspeicher
{
    m_pGainCorrection->Serialize(qds); 
    m_pPhaseCorrection->Serialize(qds);
    m_pOffsetCorrection->Serialize(qds);
}
 

void cCOM5003JustData::Deserialize(QDataStream& qds) // zum lesen aller justagedaten aus flashspeicher
{
    m_pGainCorrection->Deserialize(qds); 
    m_pPhaseCorrection->Deserialize(qds);
    m_pOffsetCorrection->Deserialize(qds);
}


quint8 cCOM5003JustData::getAdjustmentStatus()
{
    return m_pGainCorrection->getStatus() & m_pPhaseCorrection->getStatus() & m_pOffsetCorrection->getStatus();

}






