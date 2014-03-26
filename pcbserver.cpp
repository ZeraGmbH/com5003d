#include <QFile>
#include <QByteArray>
#include <QTextCodec>
#include <QList>
#include <QString>
#include <QTcpSocket>
#include <protonetpeer.h>
#include <xmlconfigreader.h>
#include <protonetserver.h>
#include <scpi.h>
#include <fcntl.h>
#include <unistd.h>
#include <netmessages.pb.h>
#include <QtDebug>

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
    myServer = new ProtoNetServer(this); // our working (talking) horse
    myServer->setDefaultWrapper(&m_ProtobufWrapper);
    connect(myServer,SIGNAL(sigClientConnected(ProtoNetPeer*)),this,SLOT(establishNewConnection(ProtoNetPeer*)));
}


void cPCBServer::establishNewConnection(ProtoNetPeer *newClient)
{
    connect(newClient,SIGNAL(sigMessageReceived(google::protobuf::Message*)),this,SLOT(executeCommand(google::protobuf::Message*)));
    // later ... connect(newClient,SIGNAL(sigMessageReceived(QByteArray*)),this,SLOT(executeCommand(QByteArray*)));
}


void cPCBServer::executeCommand(google::protobuf::Message* cmd)
{
    ProtobufMessage::NetMessage *protobufCommand;
    cSCPIObject* scpiObject;
    QString dummy;

    ProtoNetPeer* client = qobject_cast<ProtoNetPeer*>(sender());
    protobufCommand = static_cast<ProtobufMessage::NetMessage*>(cmd);

    if ( (protobufCommand != 0) && (client != 0))
    {
        if (protobufCommand->has_clientid() && protobufCommand->has_messagenr())
        {
            QByteArray clientId;
            quint32 messageNr;
            clientId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());
            messageNr = protobufCommand->messagenr();
            ProtobufMessage::NetMessage::ScpiCommand scpiCmd = protobufCommand->scpi();

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

            // dependent on rtype caller can se ack, nak, error
            // in case of error the body has to be analysed for details

            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::ack]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::nak]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::busy]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::errcon]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::erraut]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::errval]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::errxml]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::errmmem]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::errpath]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::errexec]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (m_sOutput.contains(SCPI::scpiAnswer[SCPI::errtimo]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

            Answer->set_body(m_sOutput.toStdString()); // in any case we set the body


            protobufAnswer.set_clientid(clientId, clientId.count());
            protobufAnswer.set_messagenr(messageNr);

            client->sendMessage(&protobufAnswer);
        }
        else
        {
            m_sInput =  QString::fromStdString(protobufCommand->scpi().command());
            qDebug() << m_sInput;
            if ( (scpiObject =  m_pSCPInterface->getSCPIObject(m_sInput, dummy)) != 0)
            {
                qDebug() << "1";
                m_sOutput = QString("nyet;"); // only becomes output in case of program error
                if (!scpiObject->executeSCPI(m_sInput, m_sOutput))
                {
                    qDebug() << "2";
                    m_sOutput = SCPI::scpiAnswer[SCPI::nak]+";";
                }
                else
                {
                    qDebug() << "3";
                }

            }
            else
            {
                qDebug() << "4";
                m_sOutput = SCPI::scpiAnswer[SCPI::nak]+";";
            }

            QByteArray block;

            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << (qint32)0;

            out << m_sOutput.toUtf8();
            out.device()->seek(0);
            out << (qint32)(block.size() - sizeof(qint32));

            client->getTcpSocket()->write(block);
        }
    }
}


void cPCBServer::initSCPIConnections()
{
    for (int i = 0; i < scpiConnectionList.count(); i++)
        scpiConnectionList.at(i)->initSCPIConnection("",m_pSCPInterface); // we have our interface
}


