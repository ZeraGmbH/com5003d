#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include <QObject>
#include <QList>

#include "adjflash.h"
#include "adjxml.h"
#include "resource.h"
#include "scpiconnection.h"
#include "sensechannel.h"

namespace SenseSystem
{

const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdChannelCat
};
}


class cSenseSettings;
class QDataStream;
class cClientNetBase;


class cSenseInterface : public cSCPIConnection, public cAdjFlash, public cAdjXML, public cResource
{
    Q_OBJECT

public:
    cSenseInterface(cSenseSettings* senseSettings);
    ~cSenseInterface();
    virtual void initSCPIConnection(QString leadingNodes, cSCPI* scpiInterface);
    cSenseChannel* getChannel(QString& name);
    quint8 getAdjustmentStatus(); // we return 0 if not adj. else 1
    virtual void exportAdjData(QDataStream& stream);
    virtual bool importAdjData(QString& s, QDataStream& stream);
    virtual void exportAdjData(QDomDocument& doc, QDomElement& qde); // the derived class exports adjdata beneath qdomelement
    virtual bool importAdjData(QDomNode& node);
    virtual void registerResource(cRMConnection *rmConnection, quint16 port);
    virtual void unregisterResource(cRMConnection *rmConnection);

protected slots:
    virtual void executeCommand(int cmdCode, QString& sInput, QString& sOutput);

private:
    QList<cSenseChannel*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadSenseChannelCatalog(QString& sInput);
};

#endif // SENSEINTERFACE_H
