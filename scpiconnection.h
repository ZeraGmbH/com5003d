#ifndef SCPICONNECTION_H
#define SCPICONNECTION_H

#include <QObject>
#include <QList>
#include "scpidelegate.h"

class cSCPI;


class cSCPIConnection: public QObject // pure virtual base class for scpi model interfaces
{
    Q_OBJECT

public:
    cSCPIConnection();
    virtual ~cSCPIConnection();
    virtual void initSCPIConnection(QString leadingNodes, cSCPI* scpiInterface) = 0;

protected:
    QList<cSCPIDelegate*> m_DelegateList;

protected slots:
    virtual void executeCommand(int cmdCode, QString& sInput, QString& sOutput) = 0;
};

#endif // SCPICONNECTION_H
