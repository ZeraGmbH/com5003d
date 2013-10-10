#ifndef FPZCHANNEL_H
#define FPZCHANNEL_H

#include <QObject>

#include "scpiconnection.h"


namespace FPZChannel
{
enum Commands
{
    cmdAlias,
    cmdType,
    cmdDspServer,
    cmdDspChannel,
    cmdStatus,
    cmdFormFactor
};


const double FormFactor = 21e6;
}


namespace SourceSystem
{
    class cChannelSettings;
}

class cFPZChannel : public cSCPIConnection
{
    Q_OBJECT

public:
    cFPZChannel(QString description, quint8 nr, SourceSystem::cChannelSettings* cSettings);
    virtual void initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface);

    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();

protected slots:
    virtual void executeCommand(int cmdCode, QString& sInput, QString& sOutput);

private:
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    quint16 m_nDspServer; // the dsp servers port
    quint8 m_nDspChannel; // where to find the channel's sampled data
    quint8 m_nType;
    double m_fFormFactor;
    bool m_bAvail; // is this channel available ?

    QString m_ReadAlias(QString& sInput);
    QString m_ReadType(QString& sInput);
    QString m_ReadDspServer(QString& sInput);
    QString m_ReadDspChannel(QString& sInput);
    QString m_ReadChannelStatus(QString& sInput);
    QString m_ReadFFactor(QString& sInput);
};

#endif // FPZCHANNEL_H
