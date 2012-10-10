// implemention cCOM5003JustData

#include <qdatastream.h>
#include <q3textstream.h>
#include "com5003justdata.h"





cCOM5003JustData::cCOM5003JustData()
{
    m_pPhaseCorrection = new cJustData(PhaseCorrOrder, 1.0);
    m_pGainCorrection = new cJustData(GainCorrOrder, 1.0); 
    m_pOffsetCorrection =  new cJustData(OffsetCorrOrder, 0.0);
    m_nStatus = 0; // nix justiert... nix kaputt
}


cCOM5003JustData::~cCOM5003JustData()
{
    delete m_pGainCorrection; 
    delete m_pPhaseCorrection;
    delete m_pOffsetCorrection;
}
	    

void cCOM5003JustData::Serialize(QDataStream& qds)  // zum schreiben aller justagedaten in flashspeicher
{
    m_pGainCorrection->Serialize(qds); 
    m_pPhaseCorrection->Serialize(qds);
    m_pOffsetCorrection->Serialize(qds);
    qds << m_nStatus;
}
 
void cCOM5003JustData::Deserialize(QDataStream& qds) // zum lesen aller justagedaten aus flashspeicher
{
    m_pGainCorrection->Deserialize(qds); 
    m_pPhaseCorrection->Deserialize(qds);
    m_pOffsetCorrection->Deserialize(qds);
    qds >> m_nStatus;
}


QString cCOM5003JustData::SerializeStatus()
{
    return QString("%1;").arg(m_nStatus);
}


void cCOM5003JustData::DeserializeStatus(const QString& s)
{
    m_nStatus = s.section(';',0,0).toInt();
}


void cCOM5003JustData::setStatus(int stat)
{
    m_nStatus = stat;
}
 

int cCOM5003JustData::getStatus()
{
    return m_nStatus;
}
