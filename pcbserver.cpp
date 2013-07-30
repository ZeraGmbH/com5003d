#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QList>
#include <QString>
#include <zeraserver.h>
#include <xmlconfigreader.h>
#include <zeraclient.h>
#include <scpi.h>

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


void cPCBServer::setupServer()
{
    m_pSCPInterface = new cSCPI(m_sServerName); // our scpi interface
    myServer = Zera::Net::cServer::getInstance(); // our working (talking) horse
    connect(myServer,SIGNAL(newClientAvailable(Zera::Net::cClient*)),this,SLOT(establishNewConnection(Zera::Net::cClient*)));
}


void cPCBServer::establishNewConnection(Zera::Net::cClient *newClient)
{
    connect(newClient,SIGNAL(messageReceived(QByteArray)),this,SLOT(executeCommand(QByteArray)));
}


void cPCBServer::executeCommand(const QByteArray cmd)
{
    cSCPIObject* scpiObject;
    QString dummy;

    QByteArray Test;
    QDataStream out(&Test, QIODevice::WriteOnly);
    dummy = "sense:channel:cat?";
    out << dummy;

    QDataStream in((QByteArray*) &cmd, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_0);
    in >> m_sInput;
    qDebug() << m_sInput;
    if ( (scpiObject =  m_pSCPInterface->getSCPIObject(m_sInput, dummy)) != 0)
    {
        if (!scpiObject->executeSCPI(m_sInput, m_sOutput))
            m_sOutput = SCPI::scpiAnswer[SCPI::nak];
    }
    else
        m_sOutput = SCPI::scpiAnswer[SCPI::nak];

    QByteArray block;
    block.append(m_sOutput);

    emit sendAnswer(block);
}


void cPCBServer::initSCPIConnections()
{
    for (int i = 0; i < scpiConnectionList.count(); i++)
        scpiConnectionList.at(i)->initSCPIConnection("",m_pSCPInterface); // we have our interface
}


void cPCBServer::registerResources()
{
    QDataStream stream(resourceManagerSocket); //
    for (int i = 0; i < resourceList.count(); i++)
        resourceList.at(i)->registerResource(stream);
}
