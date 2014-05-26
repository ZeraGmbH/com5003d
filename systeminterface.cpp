#include <scpi.h>
#include <scpicommand.h>

#include "com5003d.h"
#include "atmel.h"
#include "adjustment.h"
#include "scpidelegate.h"
#include "systeminfo.h"
#include "systeminterface.h"


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
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes),"DEVICE", SCPI::isQuery, scpiInterface, SystemSystem::cmdVersionDevice);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "PCB", SCPI::isQuery | SCPI::isCmdwP, scpiInterface, SystemSystem::cmdVersionPCB);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "CTRL", SCPI::isQuery, scpiInterface, SystemSystem::cmdVersionCTRL);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "FPGA", SCPI::isQuery, scpiInterface, SystemSystem::cmdVersionFPGA);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM").arg(leadingNodes), "SERIAL", SCPI::isQuery | SCPI::isCmdwP , scpiInterface, SystemSystem::cmdSerialNumber);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "BOOTLOADER", SCPI::isCmd, scpiInterface, SystemSystem::cmdUpdateControlerBootloader);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "PROGRAM", SCPI::isCmd, scpiInterface, SystemSystem::cmdUpdateControlerProgram);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "FLASH", SCPI::isCmdwP, scpiInterface, SystemSystem::cmdUpdateControlerFlash);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "EEPROM", SCPI::isCmdwP, scpiInterface, SystemSystem::cmdUpdateControlerEEprom);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "WRITE", SCPI::isCmd, scpiInterface, SystemSystem::cmdAdjFlashWrite);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "READ", SCPI::isCmd, scpiInterface, SystemSystem::cmdAdjFlashRead);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(leadingNodes), "WRITE", SCPI::isCmdwP, scpiInterface, SystemSystem::cmdAdjXMLWrite);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(leadingNodes), "READ", SCPI::isCmdwP, scpiInterface, SystemSystem::cmdAdjXMLRead);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "CHKSUM", SCPI::isQuery, scpiInterface, SystemSystem::cmdAdjFlashChksum);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SYSTEM:INTERFACE").arg(leadingNodes), "READ", SCPI::isQuery, scpiInterface, SystemSystem::cmdInterfaceRead);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
}


void cSystemInterface::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case SystemSystem::cmdVersionServer:
        sOutput = m_ReadServerVersion(sInput);
        break;
    case SystemSystem::cmdVersionDevice:
        sOutput = m_ReadDeviceVersion(sInput);
        break;
    case SystemSystem::cmdVersionPCB:
        sOutput = m_ReadWritePCBVersion(sInput);
        break;
    case SystemSystem::cmdVersionCTRL:
        sOutput = m_ReadCTRLVersion(sInput);
        break;
    case SystemSystem::cmdVersionFPGA:
        sOutput = m_ReadFPGAVersion(sInput);
        break;
    case SystemSystem::cmdSerialNumber:
        sOutput = m_ReadWriteSerialNumber(sInput);
        break;
    case SystemSystem::cmdUpdateControlerBootloader:
        sOutput = m_StartControlerBootloader(sInput);
        break;
    case SystemSystem::cmdUpdateControlerProgram:
        sOutput = m_StartControlerProgram(sInput);
        break;
    case SystemSystem::cmdUpdateControlerFlash:
        sOutput = m_LoadFlash(sInput);
        break;
    case SystemSystem::cmdUpdateControlerEEprom:
        sOutput = m_LoadEEProm(sInput);
        break;
    case SystemSystem::cmdAdjFlashWrite:
        sOutput = m_AdjFlashWrite(sInput);
        break;
    case SystemSystem::cmdAdjFlashRead:
        sOutput = m_AdjFlashRead(sInput);
        break;
    case SystemSystem::cmdAdjXMLWrite:
        sOutput = m_AdjXMLWrite(sInput);
        break;
    case SystemSystem::cmdAdjXMLRead:
        sOutput = m_AdjXMLRead(sInput);
        break;
    case SystemSystem::cmdAdjFlashChksum:
        sOutput = m_AdjFlashChksum(sInput);
        break;
    case SystemSystem::cmdInterfaceRead:
        sOutput = m_InterfaceRead(sInput);
        break;
    }
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



