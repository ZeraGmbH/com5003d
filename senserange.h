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
    cmdAlias,
    cmdAvail,
    cmdValue,
    cmdRejection,
    cmdOVRejection
};
}

class cATMEL;

class cSenseRange:public cSCPIConnection
{
    Q_OBJECT

public:
    cSenseRange(QString name, QString alias, bool avail, double rValue, double rejection, double ovrejection, quint8 rselcode, quint8 rspec);
    ~cSenseRange();
    virtual void initSCPIConnection(QString leadingNodes, cSCPI *scpiInterface);
    quint8 getAdjustmentStatus();

    QString& getName();
    quint8 getSelCode();
    cCOM5003JustData* getJustData();
    bool getAvail();
    void setAvail(bool b);


protected slots:
    virtual void executeCommand(int cmdCode, QString& sInput, QString& sOutput);


private:
    cATMEL* m_pATMEL;
    QString m_sName; // the range name
    QString m_sAlias; // the range alias name
    bool m_bAvail; // range io avail or not
    double m_fRValue; // upper range value
    double m_fRejection; // 100% rejection value
    double m_fOVRejection; // overload rejection value
    quint8 m_nSelCode; // selection code
    quint8 m_nRSpec; // range spec (phys. or virt. range)
    cCOM5003JustData* m_pJustdata;

    QString m_ReadRangeType(QString& sInput);
    QString m_ReadRangeAlias(QString& sInput);
    QString m_ReadRangeAvail(QString& sInput);
    QString m_ReadRangeValue(QString& sInput);
    QString m_ReadRangeRejection(QString& sInput);
    QString m_ReadRangeOVRejection(QString& sInput);
};


#endif // SENSERANGE_H
