#include "atmel.h"
#include "adjustment.h"
#include "statusinterface.h"


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
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"ADJUSTMENT", SCPI::isQuery, scpiInterface, StatusSystem::cmdAdjustment);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
}


void cStatusInterface::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        switch (cmdCode)
        {
        case StatusSystem::cmdDevice:
            sOutput = QString("%1;").arg(getDeviceStatus());
            break; // StatusDevice
        case StatusSystem::cmdAdjustment:
            sOutput = QString("%1;").arg(m_pAdjHandler->getAdjustmentStatus());
            break; // StatusAdjustment
        }
    }
    else
        sOutput = SCPI::scpiAnswer[SCPI::nak];
}


quint8 cStatusInterface::getDeviceStatus()
{
    QString s;

    if (pAtmel->readDeviceName(s) == cmddone) // no problem reading from atmel
        return 1; // means device available
    else
        return 0;
}





