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
    cmdMMode,
    cmdMModeCat,
    cmdChannelCat
};

enum MMode
{
    modeAC,
    modeREF,
    modeAnz
};

const QString sVoltageChannelDescription = "Measuring channel 0..480V AC";
const QString sCurrentChannelDescription = "Measuring channel 0..160A AC";
const QString sReferenceChannelDescription = "Reference channel 0..10V DC";
const QString sMMode[2] = {"AC", "REF"};
}

class cCOM5003dServer;
class cSenseSettings;
class QDataStream;
class cClientNetBase;


class cSenseInterface : public cSCPIConnection, public cAdjFlash, public cAdjXML, public cResource
{
    Q_OBJECT

public:
    cSenseInterface(cCOM5003dServer* server,cSenseSettings* senseSettings);
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
    cCOM5003dServer* m_pMyServer;
    QList<cSenseChannel*> m_ChannelList;
    QString m_sVersion;
    quint8 m_nMMode;

    void ChangeSenseMode();

    QString m_ReadVersion(QString& sInput);
    QString m_ReadWriteMModeVersion(QString& sInput);
    QString m_ReadMModeCatalog(QString& sInput);
    QString m_ReadSenseChannelCatalog(QString& sInput);
};

#endif // SENSEINTERFACE_H
