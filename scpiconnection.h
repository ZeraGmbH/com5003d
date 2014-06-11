#ifndef SCPICONNECTION_H
#define SCPICONNECTION_H

#include <QObject>
#include <QList>
#include "scpidelegate.h"

class cSCPI;
class cNotificationString;


class cSCPIConnection: public QObject // pure virtual base class for scpi model interfaces
{
    Q_OBJECT

public:
    cSCPIConnection(QObject* parent=0);
    virtual ~cSCPIConnection();
    virtual void initSCPIConnection(QString leadingNodes, cSCPI* scpiInterface) = 0;

signals:
    void notifier(cNotificationString* notifier);

protected:
    QList<cSCPIDelegate*> m_DelegateList;

protected slots:
    virtual void executeCommand(int cmdCode, QString& sInput, QString& sOutput) = 0;
};

#endif // SCPICONNECTION_H
