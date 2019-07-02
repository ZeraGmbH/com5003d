#include <qdebug.h>
#include <netmessages.pb.h>

#include "com5003dprotobufwrapper.h"


cCom5003dProtobufWrapper::cCom5003dProtobufWrapper()
{
}


std::shared_ptr<google::protobuf::Message> cCom5003dProtobufWrapper::byteArrayToProtobuf(QByteArray bA)
{
    ProtobufMessage::NetMessage *intermediate = new ProtobufMessage::NetMessage();
    if(!intermediate->ParseFromArray(bA, bA.size()))
    {
        ProtobufMessage::NetMessage::ScpiCommand *cmd = intermediate->mutable_scpi();
        cmd->set_command(bA.data(), bA.size() );
    }
    std::shared_ptr<google::protobuf::Message> proto {intermediate};
    return proto;
}


QByteArray cCom5003dProtobufWrapper::protobufToByteArray(const google::protobuf::Message &pMessage)
{
    return QByteArray(pMessage.SerializeAsString().c_str(), pMessage.ByteSize());
}


