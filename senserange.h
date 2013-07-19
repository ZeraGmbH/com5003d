#ifndef SENSERANGE_H
#define SENSERANGE_H

#include <QObject>
#include <scpi.h>

#include "scpiconnection.h"
#include "com5003justdata.h"

namespace SenseRange
{
enum Type
{
    Phys, // we distinguish between physical
    Virt  // and virtual ranges
};

enum Commands
{
    cmdType,
    cmdValue,
    cmdRejection
};
}

class cSenseRange:public cSCPIConnection
{
    Q_OBJECT

public:
    cSenseRange(QString name, double rValue, double rejection, quint8 rselcode, quint8 rspec);
    ~cSenseRange();
    virtual void initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface);
    quint8 getAdjustmentStatus();

    QString& getName();
    quint8 getSelCode();
    cCOM5003JustData* getJustData();

protected slots:
    virtual void executeCommand(int cmdCode, QString& sInput, QString& sOutput);

private:
    QString m_sName; // the range name
    double m_fRValue; // upper range value
    double m_fRejection; // 100% rejection value
    quint8 m_nSelCode; // selection code
    quint8 m_nRSpec; // range spec (phys. or virt. range
    cCOM5003JustData* m_pJustdata;

    QString m_ReadRangeType(QString& sInput);
    QString m_ReadRangeValue(QString& sInput);
    QString m_ReadRangeRejection(QString& sInput);
};


#endif // SENSERANGE_H
