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
#include <netmessages.pb.h>

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
    ProtobufMessage::NetMessage protobufCommand;
    cSCPIObject* scpiObject;
    QByteArray block;
    QString dummy;

    Zera::Net::cClient* client = qobject_cast<Zera::Net::cClient*>(sender());
    if (protobufCommand.ParseFromArray(cmd, cmd.count()))
    {
        QByteArray clientId;
        quint32 messageNr;
        clientId = QByteArray(protobufCommand.clientid().c_str(), protobufCommand.clientid().size());
        messageNr = protobufCommand.messagenr();
        ProtobufMessage::NetMessage::ScpiCommand scpiCmd = protobufCommand.scpi();

        m_sInput = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());

        if ( (scpiObject =  m_pSCPInterface->getSCPIObject(m_sInput, dummy)) != 0)
        {
            if (!scpiObject->executeSCPI(m_sInput, m_sOutput))
                m_sOutput = SCPI::scpiAnswer[SCPI::nak]+";";
        }
        else
            m_sOutput = SCPI::scpiAnswer[SCPI::nak]+";";

        ProtobufMessage::NetMessage protobufAnswer;
        ProtobufMessage::NetMessage::NetReply *Answer = protobufAnswer.mutable_reply();
        Answer->set_body(m_sOutput.toStdString());
        if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::nak]))
            Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
        else
            Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
        protobufAnswer.set_clientid(clientId, clientId.count());
        protobufAnswer.set_messagenr(messageNr);

        if (client)
        {
            block = client->translatePB2ByteArray(Answer);
            client->writeClient(block);
        }
    }
    else
    {
        m_sInput = QString::fromUtf8(cmd.data(),cmd.size());
        if ( (scpiObject =  m_pSCPInterface->getSCPIObject(m_sInput, dummy)) != 0)
        {
            m_sOutput = QString("nyet;"); // only becomes output in case of program error
            if (!scpiObject->executeSCPI(m_sInput, m_sOutput))
                m_sOutput = SCPI::scpiAnswer[SCPI::nak]+";";
        }
        else
            m_sOutput = SCPI::scpiAnswer[SCPI::nak]+";";

        block = m_sOutput.toUtf8();
        if (client)
            client->writeClient(block);
    }
}


void cPCBServer::initSCPIConnections()
{
    for (int i = 0; i < scpiConnectionList.count(); i++)
        scpiConnectionList.at(i)->initSCPIConnection("",m_pSCPInterface); // we have our interface
}


