#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <QTimer>

#include "com5003dglobal.h"
#include "atmelwatcher.h"


cAtmelWatcher::cAtmelWatcher(quint8 dlevel, QString devNode, int timeout, int tperiod)
    :m_sDeviceNode(devNode), m_nDebugLevel(dlevel)
{
    m_TimerTO.setSingleShot(true);
    m_TimerTO.setInterval(timeout);
    connect(&m_TimerTO, SIGNAL(timeout()), this, SLOT(doTimeout()));
    m_TimerPeriod.setSingleShot(false);
    m_TimerPeriod.setInterval(tperiod);
}


cAtmelWatcher::~cAtmelWatcher()
{
}


void cAtmelWatcher::start()
{
    int ret;
    QByteArray ba = m_sDeviceNode.toLatin1();
    if ( (ret = (fd = open(ba.data(),O_RDWR))) < 0 )
    {
        if (DEBUG1)  syslog(LOG_ERR,"error %d opening fpga device: '%s'\n", ret, ba.data());
        emit timeout(); // if we cannot read the device node we are ready
    }
    else
    {
        m_TimerTO.start();
        m_TimerPeriod.start();
        connect(&m_TimerPeriod, SIGNAL(timeout()), this, SLOT(doAtmelTest()));
    }
}


void cAtmelWatcher::doAtmelTest()
{
    ulong pcbTestReg;
    int r;
    QByteArray ba = m_sDeviceNode.toLatin1();
    if (lseek(fd,0xffc,0) < 0 )
    {
        if  (DEBUG1)  syslog(LOG_ERR,"error positioning fpga device: %s\n", ba.data());
    }
    else
    {
        r = read(fd,(char*) &pcbTestReg,4);

        if (DEBUG2)
            syslog(LOG_ERR,"reading fpga adr 0xffc =  %ld\n", pcbTestReg);

        if (r < 0 )
        {
            if (DEBUG1)  syslog(LOG_ERR,"error reading fpga device: %s\n",ba.data());
        }
        else
        {
            if ((pcbTestReg & 1) > 0)
            {
                m_TimerTO.disconnect(SIGNAL(timeout()));
                m_TimerPeriod.disconnect(SIGNAL(timeout()));
                m_TimerTO.stop();
                m_TimerPeriod.stop();
                close(fd);
                emit running();
            }
        }
    }
}


void cAtmelWatcher::doTimeout()
{
    m_TimerPeriod.disconnect(SIGNAL(timeout()));
    m_TimerPeriod.stop();
    close(fd);
    emit timeout();
}
