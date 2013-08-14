#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>

class cRMConnection;
class QString;

// pure virtual base class for resources to register themselves anywhere

class cResource
{

public:
    cResource();
    virtual ~cResource();
    virtual void registerResource(cRMConnection *rmConnection) = 0;
    virtual void unregisterResource(cRMConnection *rmConnection) = 0;

protected:
    void register1Resource(cRMConnection *rmConnection, QString registerParameter);
    void unregister1Resource(cRMConnection *rmConnection, QString unregisterParameter);

/*
signals:
    void rmCommand(QString& cmd);
    void registerError();
    */
};

#endif // RESOURCE_H
