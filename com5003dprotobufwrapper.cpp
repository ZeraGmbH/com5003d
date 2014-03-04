#include <qdebug.h>
#include <netmessages.pb.h>

#include "com5003dprotobufwrapper.h"


cCom5003dProtobufWrapper::cCom5003dProtobufWrapper()
{
}


google::protobuf::Message *cCom5003dProtobufWrapper::byteArrayToProtobuf(QByteArray bA)
{
    ProtobufMessage::NetMessage *proto = new ProtobufMessage::NetMessage();
    if(!proto->ParseFromArray(bA, bA.size()))
    {
        ProtobufMessage::NetMessage::ScpiCommand *cmd = proto->mutable_scpi();
        cmd->set_command(bA.data(), bA.size() );
    }
    return proto;
}


QByteArray cCom5003dProtobufWrapper::protobufToByteArray(google::protobuf::Message *pMessage)
{
    return QByteArray(pMessage->SerializeAsString().c_str(), pMessage->ByteSize());
}


