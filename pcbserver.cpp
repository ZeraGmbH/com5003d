#include <QFile>
#include <QByteArray>
#include <QTextCodec>
#include <QList>
#include <QString>
#include <zeraserver.h>
#include <xmlconfigreader.h>
#include <zeraclient.h>
#include <scpi.h>
#include <fcntl.h>
#include <unistd.h>

#include "resource.h"
#include "scpiconnection.h"
#include "pcbserver.h"
#include "com5003dglobal.h"

cPCBServer::cPCBServer(QObject *parent)
    : QObject(parent)
{
    m_sServerName = ServerName;
    m_sServerVersion = ServerVersion;
    myXMLConfigReader = new Zera::XMLConfig::cReader();
}


QString &cPCBServer::getName()
{
    return m_sServerName;
}


QString &cPCBServer::getVersion()
{
    return m_sServerVersion;
}


cSCPI *cPCBServer::getSCPIInterface()
{
    return m_pSCPInterface;
}


void cPCBServer::setupServer()
{
    m_pSCPInterface = new cSCPI(m_sServerName); // our scpi interface
    myServer = Zera::Net::cServer::getInstance(); // our working (talking) horse
    connect(myServer,SIGNAL(newClientAvailable(Zera::Net::cClient*)),this,SLOT(establishNewConnection(Zera::Net::cClient*)));
}


void cPCBServer::establishNewConnection(Zera::Net::cClient *newClient)
{
    connect(newClient,SIGNAL(messageReceived(QByteArray)),this,SLOT(executeCommand(QByteArray)));
//    connect(this, SIGNAL(sendAnswer(QByteArray)),newClient,SLOT(writeClient(QByteArray)));
}


void cPCBServer::executeCommand(const QByteArray cmd)
{
    cSCPIObject* scpiObject;
    QString dummy;
 //   quint32 sigStart;

    Zera::Net::cClient* client = qobject_cast<Zera::Net::cClient*>(sender());
    m_sInput = QString::fromUtf8(cmd.data(),cmd.size());
/*
    int nFPGAfd = open("/dev/zFPGA1reg",O_RDWR);
    lseek(nFPGAfd,0x0,0);
    sigStart = 0;
    write(nFPGAfd,(char*) &sigStart, 4);
    sigStart = 1;
    write(nFPGAfd,(char*) &sigStart, 4);
*/
    if ( (scpiObject =  m_pSCPInterface->getSCPIObject(m_sInput, dummy)) != 0)
    {
        /*
        sigStart = 2;
        write(nFPGAfd,(char*) &sigStart, 4);
        */
        m_sOutput = QString("nyet;"); // only gets output in case of program error
        if (!scpiObject->executeSCPI(m_sInput, m_sOutput))
                m_sOutput = SCPI::scpiAnswer[SCPI::nak]+";";
    }
    else
        m_sOutput = SCPI::scpiAnswer[SCPI::nak]+";";

    /*
    close(nFPGAfd);
    */

    QByteArray block;
    block = m_sOutput.toUtf8();
    //emit sendAnswer(block);
    if (client)
        client->writeClient(block);
}


void cPCBServer::initSCPIConnections()
{
    for (int i = 0; i < scpiConnectionList.count(); i++)
        scpiConnectionList.at(i)->initSCPIConnection("",m_pSCPInterface); // we have our interface
}


