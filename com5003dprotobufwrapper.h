#ifndef COM5003DPROTOBUFWRAPPER_H
#define COM5003DPROTOBUFWRAPPER_H

#include <protonetwrapper.h>

class cCom5003dProtobufWrapper : public ProtoNetWrapper
{
public:
  cCom5003dProtobufWrapper();


  google::protobuf::Message *byteArrayToProtobuf(QByteArray bA);

  QByteArray protobufToByteArray(google::protobuf::Message *pMessage);
};

#endif // COM5003DPROTOBUFWRAPPER_H
