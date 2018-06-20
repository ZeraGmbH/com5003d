#ifndef COM5003DPROTOBUFWRAPPER_H
#define COM5003DPROTOBUFWRAPPER_H

#include <xiqnetwrapper.h>

class cCom5003dProtobufWrapper : public XiQNetWrapper
{
public:
  cCom5003dProtobufWrapper();


  std::shared_ptr<google::protobuf::Message> byteArrayToProtobuf(QByteArray bA) override;

  QByteArray protobufToByteArray(const google::protobuf::Message &pMessage) override;
};

#endif // COM5003DPROTOBUFWRAPPER_H
