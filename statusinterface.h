#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include <QObject>
#include <QList>
#include <scpi.h>

#include "com5003d.h"
#include "scpiconnection.h"
#include "scpidelegate.h"

namespace StatusSystem
{

enum StatusCommands
{
    cmdDevice,
    cmdAdjustment,
    cmdAuthorization
};
}


class cAdjustment;


class cStatusInterface: public cSCPIConnection
{
    Q_OBJECT

public:
    cStatusInterface(cAdjustment* adjHandler);
    virtual void initSCPIConnection(QString leadingNodes, cSCPI* scpiInterface);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cAdjustment* m_pAdjHandler;
    quint8 getDeviceStatus();
    quint8 getAuthorizationStatus();
};

#endif // STATUSINTERFACE_H
