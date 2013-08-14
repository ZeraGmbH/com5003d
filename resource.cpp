#include <QString>

#include "resource.h"
#include "rmconnection.h"


cResource::cResource()
{
}


cResource::~cResource()
{
}


void cResource::register1Resource(cRMConnection *rmConnection, QString registerParameter)
{
    QString cmd = QString("RESOURCE:ADD");
    rmConnection->SendCommand(cmd, registerParameter);
}


void cResource::unregister1Resource(cRMConnection *rmConnection, QString unregisterParameter)
{
    QString cmd = QString("RESOURCE:REMOVE");
    rmConnection->SendCommand(cmd, unregisterParameter);
}

