#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

namespace Adjustment
{
enum jDataStatus
{
    adjusted,
    notAdjusted,
    wrongVERS = 2,
    wrongSNR = 4
};
}

class QString;
class cAdjFlash;
class cAdjXML;
class cSystemInfo;

class cAdjustment
{
public:
    cAdjustment(cSystemInfo* sInfo, QString& devNode, quint8 dlevel, quint8 adr); //
    bool exportJDataFlash();
    bool importJDataFlash();
    bool exportJDataXML(QString& file);
    bool importJDataXML(QString& file);
    void addAdjFlashObject(cAdjFlash* obj);
    void addAdjXMLObject(cAdjXML* obj);
    quint8 getAdjustmentStatus();
    quint16 getChecksum();

private:
    QList<cAdjFlash*> m_AdjFlashList;
    QList<cAdjXML*> m_AdjXMLList;
    cSystemInfo* m_pSystemInfo;
    QString m_sDeviceNode;
    quint8 m_nDebugLevel;
    quint8 m_nI2CAdr;
    quint8 m_nAdjStatus;
    quint16 m_nChecksum;

};

#endif // ADJUSTMENT_H
