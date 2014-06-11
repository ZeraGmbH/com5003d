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
    : cSCPIConnection(parent)
{
    m_sServerName = ServerName;
    m_sServerVersion = ServerVersion;
    myXMLConfigReader = new Zera::XMLConfig::cReader();
}


void cPCBServer::initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1").arg(leadingNodes),"REGISTER",SCPI::isCmdwP,scpiInterface, PCBServer::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1").arg(leadingNodes),"UNREGISTER",SCPI::isQuery | SCPI::isCmd ,scpiInterface, PCBServer::cmdUnregister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));

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


void cPCBServer::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case PCBServer::cmdRegister:
        sOutput = m_RegisterNotifier(sInput);
        break;
    case PCBServer::cmdUnregister:
        sOutput = m_UnregisterNotifier(sInput);
        break;
    }
}


QString cPCBServer::m_RegisterNotifier(QString &sInput)
{
    bool ok;
    QString dummy;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(2))
    {
        cSCPIObject* scpiObject;
        QString query = cmd.getParam(0);

        if ( (scpiObject =  m_pSCPInterface->getSCPIObject(query, dummy)) != 0)
        {
            cNotificationData notData;

            notData.netClient = client;
            notData.clientID = clientId;
            notData.notifier = cmd.getParam(1).toInt(&ok);

            notifierRegisterList.append(notData); // we wait for a notifier signal
            if (!scpiObject->executeSCPI(query, m_sOutput))
            {
                m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                notifierRegisterList.pop_back();
            }
            else
                m_sOutput = SCPI::scpiAnswer[SCPI::ack]; // we overwrite the query's output here
        }
        else
            m_sOutput = SCPI::scpiAnswer[SCPI::nak];

    }

    return m_sOutput;
}


QString cPCBServer::m_UnregisterNotifier(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == "") )
    {
        // we have to remove all notifiers for this clientId
        // todo todo
        // notifierHashtable.remove(clientId); // we remove all notifiers from list
        m_sOutput = SCPI::scpiAnswer[SCPI::ack];
    }
    else
        m_sOutput = SCPI::scpiAnswer[SCPI::nak];

    return m_sOutput;
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

    //ProtoNetPeer* client = qobject_cast<ProtoNetPeer*>(sender());

    client = qobject_cast<ProtoNetPeer*>(sender());
    protobufCommand = static_cast<ProtobufMessage::NetMessage*>(cmd);

    if ( (protobufCommand != 0) && (client != 0))
    {
        if (protobufCommand->has_clientid() && protobufCommand->has_messagenr())
        {
            quint32 messageNr;
            clientId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());
            messageNr = protobufCommand->messagenr();
            ProtobufMessage::NetMessage::ScpiCommand scpiCmd = protobufCommand->scpi();

            m_sInput = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());

            if ( (scpiObject =  m_pSCPInterface->getSCPIObject(m_sInput, dummy)) != 0)
            {
                if (!scpiObject->executeSCPI(m_sInput, m_sOutput))
                    m_sOutput = SCPI::scpiAnswer[SCPI::nak];
            }
            else
                m_sOutput = SCPI::scpiAnswer[SCPI::nak];

            ProtobufMessage::NetMessage protobufAnswer;
            ProtobufMessage::NetMessage::NetReply *Answer = protobufAnswer.mutable_reply();

            // dependent on rtype caller can se ack, nak, error
            // in case of error the body has to be analyzed for details

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


void cPCBServer::establishNewNotifier(cNotificationString *notifier)
{
    if (notifierRegisterList.count() > 0) // if we're waiting for notifier
    {
        cNotificationData notData = notifierRegisterList.takeFirst(); // we pick the notification data
        notifierHashtable.insertMulti(notifier, notData); //
        connect(notifier, SIGNAL(valueChanged()), this, SLOT(asyncHandler()));
    }
}


void cPCBServer::asyncHandler()
{
    cNotificationString* notifier = qobject_cast<cNotificationString*>(sender());

    QList<cNotificationData> notList = notifierHashtable.values(notifier);
    if (notList.count() > 0)
    {
        ProtobufMessage::NetMessage protobufIntMessage;

        for (int i = 0; i < notList.count(); i++)
        {
            ProtobufMessage::NetMessage::NetReply *intMessage = protobufIntMessage.mutable_reply();
            cNotificationData notData = notList.at(i);
            QString s = QString("Notify:%1").arg(notData.notifier);
            intMessage->set_body(s.toStdString());
            intMessage->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
            QByteArray id = notData.clientID;
            protobufIntMessage.set_clientid(id, id.count());
            protobufIntMessage.set_messagenr(0); // interrupt

            notData.netClient->sendMessage(&protobufIntMessage);
        }
    }
}


void cPCBServer::initSCPIConnections()
{
    for (int i = 0; i < scpiConnectionList.count(); i++)
    {
        scpiConnectionList.at(i)->initSCPIConnection("",m_pSCPInterface); // we have our interface
        connect(scpiConnectionList.at(i), SIGNAL(notifier(cNotificationString*)), this, SLOT(establishNewNotifier(cNotificationString*)));
    }
}


