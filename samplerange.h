#ifndef SAMPLERANGE_H
#define SAMPLERANGE_H

#include <QObject>

#include "scpiconnection.h"

enum SampleRangeCommands
{
    SampleRangeSamples
};

class QString;
class cSCPI;
class cSCPIConnection;


class cSampleRange: public cSCPIConnection
{
    Q_OBJECT

public:
    cSampleRange(QString name, quint16 srate, quint8 selcode);
    virtual void initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface);
    QString &getName();
    quint8 getSelCode();

protected slots:
    virtual void executeCommand(int cmdCode, QString& sInput, QString& sOutput);

private:
    QString m_sName;
    quint16 m_nSRate;
    quint8 m_nSelCode; // selection code

    QString m_ReadSRate(QString &sInput);
};

#endif // SAMPLERANGE_H
