#include "atmel.h"
#include "adjustment.h"
#include "statusinterface.h"
#include "protonetcommand.h"

extern cATMEL* pAtmel;


cStatusInterface::cStatusInterface(cAdjustment *adjHandler)
    :m_pAdjHandler(adjHandler)
{
}


void cStatusInterface::initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"DEVICE",SCPI::isQuery,scpiInterface, StatusSystem::cmdDevice);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"ADJUSTMENT", SCPI::isQuery, scpiInterface, StatusSystem::cmdAdjustment);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}


void cStatusInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isQuery())
    {
        switch (cmdCode)
        {
        case StatusSystem::cmdDevice:
            protoCmd->m_sOutput = QString("%1").arg(getDeviceStatus());
            break; // StatusDevice
        case StatusSystem::cmdAdjustment:
            protoCmd->m_sOutput = QString("%1").arg(m_pAdjHandler->getAdjustmentStatus());
            break; // StatusAdjustment
        }
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


quint8 cStatusInterface::getDeviceStatus()
{
    QString s;

    if (pAtmel->readDeviceName(s) == cmddone) // no problem reading from atmel
        return 1; // means device available
    else
        return 0;
}





