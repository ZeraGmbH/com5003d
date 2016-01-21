#include <syslog.h>
#include <crcutils.h>

#include "com5003dglobal.h"
#include "i2cutils.h"
#include "atmel.h"


cATMEL::cATMEL(QString devnode, quint8 adr, quint8 debuglevel)
    :m_sI2CDevNode(devnode), m_nI2CAdr(adr), m_nDebugLevel(debuglevel)
{
    m_pCRCGenerator = new cMaxim1WireCRC();
}


cATMEL::~cATMEL()
{
    delete m_pCRCGenerator;
}


atmelRM cATMEL::readSerialNumber(QString& answer)
{
    return mGetText(hwGetSerialNr, answer);
}


atmelRM cATMEL::writeSerialNumber(QString &sNumber)
{
    atmelRM ret;

    quint16 len = sNumber.length();
    if ( (len<1) || (len>24))
        ret = cmdfault;
    else
    {
        QByteArray ba = sNumber.toLatin1();

        struct hw_cmd CMD = { cmdcode: hwSetSerialNr, device: 0, par: (quint8*) ba.data(), plen: len,cmdlen: 0,cmddata: 0, RM:0 };

        if ( (writeCommand(&CMD) == 0) && (CMD.RM == 0) )
            ret = cmddone;
        else
            ret = cmdexecfault;
    }

    return ret;
}


atmelRM cATMEL::readDeviceName(QString& answer)
{
    return mGetText(hwGetDevName, answer);
}


atmelRM cATMEL::readPCBVersion(QString& answer)
{
    return mGetText(hwGetPCBVersion, answer);
}


atmelRM cATMEL::writePCBVersion(QString &sVersion)
{
    atmelRM ret;

    quint16 len = sVersion.length();
    if ( (len<1) || (len>24))
        ret = cmdfault;
    else
    {
        QByteArray ba = sVersion.toLatin1();
        struct hw_cmd CMD = { cmdcode: hwSetPCBVersion, device: 0, par: (quint8*) ba.data(), plen: len,cmdlen: 0,cmddata: 0, RM:0 };

        if ( (writeCommand(&CMD) == 0) && (CMD.RM == 0) )
            ret = cmddone;
        else
            ret = cmdexecfault;
    }

    return ret;
}


atmelRM cATMEL::readCTRLVersion(QString& answer)
{
    return mGetText(hwGetCtrlVersion, answer);
}


atmelRM cATMEL::readLCAVersion(QString& answer)
{
    return mGetText(hwGetLCAVersion, answer);
}


atmelRM cATMEL::startBootLoader()
{
    quint8 PAR[1];
    atmelRM ret;

    struct hw_cmd CMD = {cmdcode: hwStartBootloader, device: 0, par: PAR, plen: 0,cmdlen: 0,cmddata: 0, RM:0 };

    if ( (writeCommand(&CMD) == 0) && (CMD.RM == 0) ) // bootloader started ...
        ret = cmddone;
    else
        ret = cmdexecfault;

    return ret;
}


atmelRM cATMEL::startProgram()
{
    quint8 PAR[1];
    atmelRM ret;

    struct bl_cmd blStartProgramCMD = {cmdcode: blStartProgram, par:  PAR, plen: 0, cmdlen: 0, cmddata: 0, RM:  0};

    if ( (writeBootloaderCommand(&blStartProgramCMD) != 0) || (blStartProgramCMD.RM) )
        ret = cmdexecfault;
    else
        ret = cmddone;

    return ret;
}


atmelRM cATMEL::loadFlash(cIntelHexFileIO &ihxFIO)
{
    return loadMemory(blWriteFlashBlock, ihxFIO);
}


atmelRM cATMEL::loadEEprom(cIntelHexFileIO &ihxFIO)
{
    return loadMemory(blWriteEEPromBlock, ihxFIO);
}


atmelRM cATMEL::readChannelStatus(quint8 channel, quint8 &stat)
{
    quint8 PAR[1];
    char answ[2];

    struct hw_cmd CMD = { cmdcode: hwGetStatus, device: channel, par: PAR, plen: 0,cmdlen: 0,cmddata: 0, RM:0 };

    if  ( (writeCommand(&CMD) == 2) && (CMD.RM == 0) &&  (readOutput(answ,2) == 2) )
    {
        stat = answ[0];
        return cmddone;
    }
    else
        return cmdexecfault;
}


atmelRM cATMEL::readCriticalStatus(quint16 &stat)
{
    quint8 PAR[1];
    char answ[3];

    struct hw_cmd CMD = { cmdcode: hwGetCritStat, device: 0, par: PAR, plen: 0,cmdlen: 0,cmddata: 0, RM:0 };

    if  ( (writeCommand(&CMD) == 3) && (CMD.RM == 0) &&  (readOutput(answ,3) == 3) )
    {
         stat = (answ[0] << 8) + answ[1];
         return cmddone;
    }
    else
         return cmdexecfault;
}


atmelRM cATMEL::resetCriticalStatus(quint16 stat)
{
    quint8 PAR[2];

    PAR[0] = (stat >> 8) & 255;
    PAR[1] = stat & 255;

    struct hw_cmd CMD = { cmdcode: hwResetCritStat, device: 0, par: PAR, plen: 2,cmdlen: 0,cmddata: 0, RM:0 };

    if  ( (writeCommand(&CMD) == 0) && (CMD.RM == 0) )
        return cmddone;
    else
        return cmdexecfault;
}


atmelRM cATMEL::readRange(quint8 channel, quint8 &range)
{
    quint8 PAR[1];
    int rlen;

    struct hw_cmd CMD = { cmdcode: hwGetRange, device: channel, par: PAR, plen: 0,cmdlen: 0,cmddata: 0, RM:0 };

    char answ[2];

    if ( ( (rlen = writeCommand(&CMD)) == 2) && (CMD.RM == 0) && (readOutput(answ,rlen) == rlen) )
    {
        range = answ[0];
        return cmddone;
    }
    else
        return cmdexecfault;
}


atmelRM cATMEL::setRange(quint8 channel, quint8 range)
{
    quint8 PAR[1];
    PAR[0] = range;

    struct hw_cmd CMD = { cmdcode: hwSetRange, device: channel, par: PAR, plen: 1,cmdlen: 0,cmddata: 0, RM:0 };

    if ( (writeCommand(&CMD) == 0) && (CMD.RM == 0) )
        return cmddone;
    else
        return cmdexecfault;
}


atmelRM cATMEL::getEEPROMAccessEnable(bool &enable)
{
    quint8 PAR[1];
    enable = 0; // default

    struct hw_cmd CMD = { cmdcode: hwGetFlashWriteAccess, device: 0, par: PAR, plen: 0,cmdlen: 0,cmddata: 0, RM:0 };

    if ( (writeCommand(&CMD) == 2) && (CMD.RM == 0))
    {
        char answ[2];
        if (readOutput(answ,2) == 2)
            enable = (answ[0] != 0);
        return cmddone;
    }

    return cmdexecfault;
}


atmelRM cATMEL::readSamplingRange(quint8 &srange)
{
    srange = 0;
    return cmddone;
}


atmelRM cATMEL::setSamplingRange(quint8)
{
    return cmddone;
}


atmelRM cATMEL::setMeasMode(quint8 mmode)
{
    quint8 PAR[1];
    PAR[0] = mmode;

    struct hw_cmd CMD = { cmdcode: hwSetMode, device: 0, par: PAR, plen: 1,cmdlen: 0,cmddata: 0, RM:0 };

    if ( (writeCommand(&CMD) == 0) && (CMD.RM == 0) )
        return cmddone;
    else
        return cmdexecfault;
}


atmelRM cATMEL::readMeasMode(quint8 &mmode)
{
    quint8 PAR[1];
    mmode = 0; // default AC

    struct hw_cmd CMD = { cmdcode: hwGetMode, device: 0, par: PAR, plen: 0,cmdlen: 0,cmddata: 0, RM:0 };


    if ( (writeCommand(&CMD) == 2) && (CMD.RM == 0))
    {
        char answ[2];
        if (readOutput(answ,2) == 2)
            mmode = answ[0];
        return cmddone;
    }

    return cmdexecfault;

}


atmelRM cATMEL::setPLLChannel(quint8 chn)
{
        quint8 PAR[1];
        PAR[0] = chn;

        struct hw_cmd CMD = { cmdcode: hwSetPLLChannel, device: 0, par: PAR, plen: 1,cmdlen: 0,cmddata: 0, RM:0 };

        if ( (writeCommand(&CMD) == 0) && (CMD.RM == 0) )
            return cmddone;
        else
            return cmdexecfault;
}


atmelRM cATMEL::readPLLChannel(quint8& chn)
{
    quint8 PAR[1];
    chn = 0; // default AC

    struct hw_cmd CMD = { cmdcode: hwGetMode, device: 0, par: PAR, plen: 0,cmdlen: 0,cmddata: 0, RM:0 };

    if ( (writeCommand(&CMD) == 2) && (CMD.RM == 0))
    {
        char answ[2];
        if (readOutput(answ,2) == 2)
            chn = answ[0];
                return cmddone;
    }

    return cmdexecfault;
}


atmelRM cATMEL::mGetText(hw_cmdcode hwcmd, QString& answer)
{
    quint8 PAR[1];
    int rlen;
    atmelRM ret = cmdexecfault;

    struct hw_cmd CMD = { cmdcode: hwcmd, device: 0, par: PAR, plen: 0,cmdlen: 0,cmddata: 0, RM:0 };


    if ( ( (rlen = writeCommand(&CMD)) > 0) && (CMD.RM == 0))
    {
        char answ[rlen];
        if (readOutput(answ,rlen) == rlen)
        {
            answ[rlen-1] = 0;
            answer = (char*)answ;
            ret = cmddone;
        }
    }

    return ret;
}


qint16 cATMEL::writeCommand(hw_cmd * hc)
{

    quint16 rlen = -1; // return value ; < 0 means error
    quint8 inpBuf[5]; // the answer always has 5 bytes

    GenCommand(hc);

    struct i2c_msg Msgs[2] = { {addr :m_nI2CAdr, flags: 0,len: hc->cmdlen, buf: hc->cmddata}, // 2 messages (tagged format )
                               {addr :m_nI2CAdr, flags: (I2C_M_RD+I2C_M_NOSTART), len: 5, buf: inpBuf} };

    struct i2c_rdwr_ioctl_data comData = {  msgs: Msgs, nmsgs: 2 };

    if DEBUG2 syslog(LOG_INFO,"i2c writecommand %d bytes to i2cslave at 0x%x",hc->cmdlen,m_nI2CAdr);

    if (! I2CTransfer(m_sI2CDevNode,m_nI2CAdr,m_nDebugLevel ,&comData)) // if no error
    {
        if (inpBuf[4] == m_pCRCGenerator->CalcBlockCRC(inpBuf, 4) )
        {
            rlen = (inpBuf[2] << 8) + inpBuf[3];
            hc->RM = (inpBuf[0] << 8) + inpBuf[1];
            if (hc->RM)
                if DEBUG1 syslog(LOG_ERR,"i2ctransfer error 0x%x with i2cslave at 0x%x failed",hc->RM,m_nI2CAdr);
        }
    }
    else
    {
        if DEBUG1 syslog(LOG_ERR,"i2ctransfer to i2cslave at 0x%x failed", m_nI2CAdr);
    }
    delete hc->cmddata;
    return rlen; // return -1  on error else length info
}


qint16 cATMEL::writeBootloaderCommand(bl_cmd* blc)
{
    int rlen = -1; // rückgabewert länge ; < 0 bedeutet fehler
    quint8 inpBuf[5]; // die antwort auf command ist immer 5 bytes lang

    GenBootloaderCommand(blc);
    struct i2c_msg Msgs[2] = { {addr :m_nI2CAdr, flags: 0,len: blc->cmdlen, buf: blc->cmddata}, // 2 messages (tagged format )
                               {addr :m_nI2CAdr, flags: (I2C_M_RD+I2C_M_NOSTART), len: 5, buf: inpBuf} };

    struct i2c_rdwr_ioctl_data comData = {  msgs: Msgs, nmsgs: 2 };

    if DEBUG2 syslog(LOG_INFO,"write bootloader command %d bytes to i2cslave at 0x%x",blc->cmdlen,m_nI2CAdr);

    if (! I2CTransfer(m_sI2CDevNode,m_nI2CAdr,m_nDebugLevel ,&comData)) // wenn kein fehler
    {
        if (inpBuf[4] == m_pCRCGenerator->CalcBlockCRC(inpBuf, 4) )
        {
            rlen = (inpBuf[2] << 8) + inpBuf[3];
            blc->RM = (inpBuf[0] << 8) + inpBuf[1];
            if (blc->RM)
                if DEBUG1 syslog(LOG_ERR,"i2ctransfer error 0x%x with i2cslave at 0x%x failed",blc->RM,m_nI2CAdr);
        }
    }
    else
    {
        if DEBUG1 syslog(LOG_ERR,"i2ctransfer to i2cslave at 0x%x failed",m_nI2CAdr);
    }

    delete blc->cmddata;
    return rlen; // -1 wenn fehler ; sonst länge des erzeugten output
}


qint16 cATMEL::readOutput(char* data, quint16 dlen)
{
    int rlen = -1; // return value ; < 0 means error

    struct i2c_msg Msgs = {addr :m_nI2CAdr, flags: I2C_M_RD,len: dlen,buf: (uchar*) data}; // 1 message
    struct i2c_rdwr_ioctl_data comData = {  msgs: &Msgs, nmsgs: 1 };

    if DEBUG2 syslog(LOG_INFO,"i2c readoutput %d bytes from i2cslave at 0x%x",dlen+1, m_nI2CAdr);
    if (! I2CTransfer(m_sI2CDevNode,m_nI2CAdr,m_nDebugLevel,&comData)) // if no error
    {
        if (data[dlen-1] == m_pCRCGenerator->CalcBlockCRC((quint8*) data, dlen-1) )
        rlen = dlen;
     }
     else
     {
        if DEBUG1 syslog(LOG_ERR,"i2ctransfer to i2cslave at 0x%x failed", m_nI2CAdr);
     }
     return rlen; // return -1  on error else length info

}


void  cATMEL::GenCommand(hw_cmd* hc)
{
    qint16 len = 6 + hc->plen;
    quint8* p = new quint8[len];
    hc->cmddata = p;

    *p++ = len >> 8;
    *p++ = len & 0xFF;
    *p++ = ((hc->cmdcode) >> 8) & 0xFF;
    *p++ = (hc->cmdcode) & 0xFF;
    *p++ = hc->device;

    const quint8* ppar = hc->par;
    for (int i = 0; i < hc->plen;i++)
    *p++ = *ppar++;

        *p = m_pCRCGenerator->CalcBlockCRC(hc->cmddata,len-1);
        hc->cmdlen = len;
}


void cATMEL::GenBootloaderCommand(bl_cmd* blc)
{
    quint16 len = 4 + blc->plen;
    quint8* p = new quint8[len];
    blc->cmddata = p;

    *p++ = blc->cmdcode;
    *p++ = blc->plen >> 8;
    *p++ = blc->plen & 0xFF;

    quint8* ppar = blc->par;
    for (int i = 0; i < blc->plen; i++)
        *p++ = *ppar++;

    *p = m_pCRCGenerator->CalcBlockCRC(blc->cmddata,len-1);
    blc->cmdlen = len;
}


quint8* cATMEL::GenAdressPointerParameter(quint8 adresspointerSize, quint32 adr)
{
    quint8* par = new quint8(adresspointerSize);
    quint8* pptr = par;
    for (int  i = 0; i < adresspointerSize; i++)
    *pptr++ = (quint8) ((adr >> (8* ( (adresspointerSize-1) - i)) ) & 0xff);
    return par;
}


atmelRM cATMEL::loadMemory(bl_cmdcode blwriteCmd, cIntelHexFileIO& ihxFIO)
{
    atmelRM  ret = cmddone;
    quint8 PAR[1];
    struct bl_cmd blInfoCMD = {cmdcode: blReadInfo, par:  PAR, plen: 0, cmdlen: 0, cmddata: 0, RM:  0};

    quint16 dlen = writeBootloaderCommand(&blInfoCMD);

    if ( (dlen > 5) && (blInfoCMD.RM == 0) ) // we must get at least 6 bytes
    { // we've got them and no error
        dlen++; // dlen is only data lenght but we also want the crc-byte
        char blInput[255];
        int read = readOutput(blInput, dlen);
        if ( read == dlen) // we got the reqired information from bootloader
        {
            blInfo BootloaderInfo;
            quint8* dest = (quint8*) &BootloaderInfo;
            int pos = strlen(blInput);
            int i;
            for (i = 0; i < 4; i++)
                dest[i ^ 1] = blInput[pos+1+i]; // little endian ... big endian

            dest[i] = blInput[pos+1+i];
            quint32 MemAdress = 0;
            quint32 MemOffset;
            QByteArray MemByteArray;

            ihxFIO.GetMemoryBlock( BootloaderInfo.MemPageSize, MemAdress, MemByteArray, MemOffset);
            while ( (MemByteArray.count()) && (ret == cmddone) ) // as long we get data from hexfile
            {
                quint8* adrParameter;
                quint16 adrParLen = BootloaderInfo.AdressPointerSize;
                adrParameter = GenAdressPointerParameter(adrParLen, MemAdress);

                struct bl_cmd blAdressCMD = {cmdcode: blWriteAddressPointer, par: adrParameter, plen: adrParLen, cmdlen: 0, cmddata: 0, RM: 0};

                if ( (writeBootloaderCommand(&blAdressCMD) == 0) && (blAdressCMD.RM == 0) )
                { // we were able to write the adress
                    quint8* memdat = (quint8*)MemByteArray.data();
                    quint16 memlen = MemByteArray.count();

                    struct bl_cmd blwriteMemCMD = {cmdcode: blwriteCmd, par: memdat, plen: memlen, cmdlen: 0, cmddata: 0, RM: 0};

                    if ( (writeBootloaderCommand(&blwriteMemCMD) == 0) && (blwriteMemCMD.RM == 0) )
                    { // we were able to write the data and expect the data to be in flash when sent over i2c
                        MemAdress += BootloaderInfo.MemPageSize;
                        ihxFIO.GetMemoryBlock( BootloaderInfo.MemPageSize, MemAdress, MemByteArray, MemOffset); // versuch weitere daten aus hexfile zu lesen
                    }
                    else
                        ret = cmdexecfault;
                }
                else
                    ret = cmdexecfault;

                delete adrParameter;
            }
        }
        else
            ret = cmdexecfault;
    }
    else
        ret = cmdexecfault;

    return ret;
}
