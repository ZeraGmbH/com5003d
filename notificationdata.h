#ifndef NOTIFICATIONDATA_H
#define NOTIFICATIONDATA_H

#include <QByteArray>
#include <protonetpeer.h>

struct cNotificationData
{
    ProtoNetPeer *netClient;
    QByteArray clientID;
    quint16 notifier;
};

#endif // NOTIFICATIONDATA_H
