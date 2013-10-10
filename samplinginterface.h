#ifndef SAMPLINGINTERFACE_H
#define SAMPLINGINTERFACE_H

#include <QObject>
#include <QList>

#include "samplerange.h"
#include "resource.h"

namespace SamplingSystem
{

const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdChannelCat,
    cmdChannelAlias,
    cmdChannelType,
    cmdChannelMode,
    cmdChannelStatus,
    cmdChannelRange,
    cmdChannelRangeCat
};

}

class QString;
class cSCPIConnection;
class cSamplingSettings;
class cClientNetBase;

class cSamplingInterface:public cSCPIConnection, public cResource
{
    Q_OBJECT

public:
    cSamplingInterface(cSamplingSettings* samplingSettings);
    virtual void initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface);
    virtual void registerResource(cRMConnection *rmConnection, quint16 port);
    virtual void unregisterResource(cRMConnection *rmConnection);

protected slots:
    virtual void executeCommand(int cmdCode, QString& sInput, QString& sOutput);

private:
    quint16 m_nType;
    QString m_sVersion;
    QString m_sAlias;
    QString m_sName; // the samplingsystem's name
    QString m_sDescription; // the samplingsystem's brief description
    bool m_bAvail; // is this sampling system available ?
    QList<cSampleRange*> m_SampleRangeList;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadSamplingChannelCatalog(QString& sInput);
    QString m_ReadAlias(QString&sInput);
    QString m_ReadType(QString&sInput);
    QString m_ReadWriteMode(QString& sInput);
    QString m_ReadStatus(QString& sInput);
    QString m_ReadWriteSamplingRange(QString& sInput);
    QString m_ReadSamplingRangeCatalog(QString& sInput);
};

#endif // SAMPLINGINTERFACE_H
