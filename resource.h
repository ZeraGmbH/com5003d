#ifndef RESOURCE_H
#define RESOURCE_H

class QDataStream;


// pure virtual base class for resources to register themselves anywhere

class cResource
{
public:
    cResource();
    virtual void registerResource(QDataStream& stream) = 0;
};

#endif // RESOURCE_H
