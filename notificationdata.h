#ifndef NOTIFICATIONDATA_H
#define NOTIFICATIONDATA_H

#include <QByteArray>
#include <protonetpeer.h>

#include "notificationstring.h"

struct cNotificationData
{
    ProtoNetPeer *netClient;
    QByteArray clientID;
    quint16 notifier;
    cNotificationString *notString;
};

#endif // NOTIFICATIONDATA_H
