#include "pcbserver.h"
#include "debug.h"


cPCBServer::cPCBServer()
    :m_sServerName(ServerBasisName)
{
    *m_pSCPInterface = new cSCPI(m_sServerName); // our scpi interface
    m_nDebugLevel = NoDebug; // default no debugging information
}


QString &cPCBServer::getName()
{
    return m_sServerName;
}

