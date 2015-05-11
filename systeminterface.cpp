#include <scpi.h>
#include <scpicommand.h>

#include "com5003d.h"
#include "atmel.h"
#include "adjustment.h"
#include "scpidelegate.h"
#include "systeminfo.h"
#include "systeminterface.h"
#include "protonetcommand.h"

extern cATMEL* pAtmel;

cSystemInterface::cSystemInterface(cCOM5003dServer *server, cAdjustment *adjHandler, cSystemInfo *sInfo)
    :m_pMyServer(server), m_pAdjHandler(adjHandler), m_pSystemInfo(sInfo)
{
}


void cSystemInterface::initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes),"SERVER", SCPI::isQuery, scpiInterface, SystemSystem::cmdVersionServer);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes),"DEVICE", SCPI::isQuery, scpiInterface, SystemSystem::cmdVersionDevice);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "PCB", SCPI::isQuery | SCPI::isCmdwP, scpiInterface, SystemSystem::cmdVersionPCB);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "CTRL", SCPI::isQuery, scpiInterface, SystemSystem::cmdVersionCTRL);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "FPGA", SCPI::isQuery, scpiInterface, SystemSystem::cmdVersionFPGA);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM").arg(leadingNodes), "SERIAL", SCPI::isQuery | SCPI::isCmdwP , scpiInterface, SystemSystem::cmdSerialNumber);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "BOOTLOADER", SCPI::isCmd, scpiInterface, SystemSystem::cmdUpdateControlerBootloader);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "PROGRAM", SCPI::isCmd, scpiInterface, SystemSystem::cmdUpdateControlerProgram);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "FLASH", SCPI::isCmdwP, scpiInterface, SystemSystem::cmdUpdateControlerFlash);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "EEPROM", SCPI::isCmdwP, scpiInterface, SystemSystem::cmdUpdateControlerEEprom);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "WRITE", SCPI::isCmd, scpiInterface, SystemSystem::cmdAdjFlashWrite);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "READ", SCPI::isCmd, scpiInterface, SystemSystem::cmdAdjFlashRead);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(leadingNodes), "WRITE", SCPI::isCmdwP, scpiInterface, SystemSystem::cmdAdjXMLWrite);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(leadingNodes), "READ", SCPI::isCmdwP, scpiInterface, SystemSystem::cmdAdjXMLRead);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "CHKSUM", SCPI::isQuery, scpiInterface, SystemSystem::cmdAdjFlashChksum);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:INTERFACE").arg(leadingNodes), "READ", SCPI::isQuery, scpiInterface, SystemSystem::cmdInterfaceRead);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}


void cSystemInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SystemSystem::cmdVersionServer:
        protoCmd->m_sOutput = m_ReadServerVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdVersionDevice:
        protoCmd->m_sOutput = m_ReadDeviceVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdVersionPCB:
        protoCmd->m_sOutput = m_ReadWritePCBVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdVersionCTRL:
        protoCmd->m_sOutput = m_ReadCTRLVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdVersionFPGA:
        protoCmd->m_sOutput = m_ReadFPGAVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdSerialNumber:
        protoCmd->m_sOutput = m_ReadWriteSerialNumber(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdUpdateControlerBootloader:
        protoCmd->m_sOutput = m_StartControlerBootloader(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdUpdateControlerProgram:
        protoCmd->m_sOutput = m_StartControlerProgram(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdUpdateControlerFlash:
        protoCmd->m_sOutput = m_LoadFlash(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdUpdateControlerEEprom:
        protoCmd->m_sOutput = m_LoadEEProm(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjFlashWrite:
        protoCmd->m_sOutput = m_AdjFlashWrite(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjFlashRead:
        protoCmd->m_sOutput = m_AdjFlashRead(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjXMLWrite:
        protoCmd->m_sOutput = m_AdjXMLWrite(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjXMLRead:
        protoCmd->m_sOutput = m_AdjXMLRead(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjFlashChksum:
        protoCmd->m_sOutput = m_AdjFlashChksum(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdInterfaceRead:
        protoCmd->m_sOutput = m_InterfaceRead(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString cSystemInterface::m_ReadServerVersion(QString &sInput)
{
    QString s;
    cSCPICommand cmd = sInput;

    if ( cmd.isQuery() )
    {
        s = m_pMyServer->getVersion();
    }
    else
        s = SCPI::scpiAnswer[SCPI::nak];

    return s;
}


QString cSystemInterface::m_ReadDeviceVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getDeviceVersion();
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSystemInterface::m_ReadDeviceName(QString& sInput)
{
    QString s;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getDeviceName();
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSystemInterface::m_ReadWritePCBVersion(QString &sInput)
{
    QString s;
    int ret = cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_pSystemInfo->dataRead())
            s = m_pSystemInfo->getPCBVersion();
        else
            s = SCPI::scpiAnswer[SCPI::errexec];
    }
    else
    {
        if (cmd.isCommand(1))
        {
            QString Version = cmd.getParam(0);
            ret = pAtmel->writePCBVersion(Version);
            m_pSystemInfo->getSystemInfo(); // read back info
        }

        m_genAnswer(ret, s);
    }

    return s;
}


QString cSystemInterface::m_ReadCTRLVersion(QString &sInput)
{
    QString s;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getCTRLVersion();
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }

    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSystemInterface::m_ReadFPGAVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getLCAVersion();
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }

    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSystemInterface::m_ReadWriteSerialNumber(QString &sInput)
{
    atmelRM ret = cmdfault;
    QString s;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        {
            if (m_pSystemInfo->dataRead())
                s = m_pSystemInfo->getSerialNumber();
            else
                s = SCPI::scpiAnswer[SCPI::errexec];
        }
    }
    else
    {
        if (cmd.isCommand(1))
        {
            QString Serial = cmd.getParam(0);
            ret = pAtmel->writeSerialNumber(Serial);
            m_pSystemInfo->getSystemInfo(); // read back info
        }

        m_genAnswer(ret, s);
    }

    return s;
}


QString cSystemInterface::m_StartControlerBootloader(QString& sInput)
{
    QString s;
    int ret = cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        ret = pAtmel->startBootLoader();
    }
    m_genAnswer(ret, s);
    return s;
}


QString cSystemInterface::m_StartControlerProgram(QString &sInput)
{
    QString s;
    int ret = cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        ret = pAtmel->startProgram();
    }
    m_genAnswer(ret, s);
    return s;
}


QString cSystemInterface::m_LoadFlash(QString &sInput)
{
    QString s;
    int ret = cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1))
    {
        QString filename = cmd.getParam(0);
        cIntelHexFileIO IntelHexData;
        if (IntelHexData.ReadHexFile(filename))
        {
           ret = pAtmel->loadFlash(IntelHexData);
        }
        else
            ret = cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}


QString cSystemInterface::m_LoadEEProm(QString &sInput)
{
    QString s;
    int ret = 1;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1))
    {
        QString filename = cmd.getParam(0);
        cIntelHexFileIO IntelHexData;
        if (IntelHexData.ReadHexFile(filename))
        {
            ret = pAtmel->loadEEprom(IntelHexData);
        }
        else
            ret = cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}


QString cSystemInterface::m_AdjFlashWrite(QString &sInput)
{
    QString s;
    int ret = cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        if (m_pAdjHandler->exportJDataFlash())
            ret = cmddone;
        else
            ret = cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}


QString cSystemInterface::m_AdjFlashRead(QString &sInput)
{
    QString s;
    int ret = cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        if (m_pAdjHandler->importJDataFlash())
            ret = cmddone;
        else
            ret = cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}


QString cSystemInterface::m_AdjXMLWrite(QString &sInput)
{
    QString s;
    int ret = cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1))
    {
        QString filename = cmd.getParam(0);
        if (m_pAdjHandler->exportJDataXML(filename))
            ret = cmddone;
        else
            ret = cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}


QString cSystemInterface::m_AdjXMLRead(QString &sInput)
{
    QString s;
    int ret = cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1))
    {
        bool enable = false;
        pAtmel->getEEPROMAccessEnable(enable);
        if (enable)
        {
            QString filename = cmd.getParam(0);
            if (m_pAdjHandler->importJDataXML(filename))
                ret = cmddone;
            else
                ret = cmdexecfault;
        }
        else
            return SCPI::scpiAnswer[SCPI::erraut];
    }

    m_genAnswer(ret, s);
    return s;
}


QString cSystemInterface::m_AdjFlashChksum(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s = QString("%1").arg(m_pAdjHandler->getAdjustmentStatus());
        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSystemInterface::m_InterfaceRead(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s;
        m_pMyServer->getSCPIInterface()->exportSCPIModelXML(s);
        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


void cSystemInterface::m_genAnswer(int select, QString &answer)
{
    switch (select)
    {
    case cmddone:
        answer = SCPI::scpiAnswer[SCPI::ack];
        break;
    case cmdfault:
        answer = SCPI::scpiAnswer[SCPI::nak];
        break;
    case cmdexecfault:
        answer = SCPI::scpiAnswer[SCPI::errexec];
        break;
    }
}



