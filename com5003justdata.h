// header datei com5003justdata.h

#ifndef COM5003JUSTDATA_H
#define COM5003JUSTDATA_H

#include <qdatastream.h>
#include <q3textstream.h>
#include "justdata.h"


enum jDataStatus { wrongVERS = 2, wrongSNR = 4};

const int GainCorrOrder = 1;
const int PhaseCorrOrder  = 3;
const int OffsetCorrOrder = 0;
	

class cCOM5003JustData { // alle korrekturdaten für einen bereich + status
public:
    cCOM5003JustData();
    ~cCOM5003JustData();
    
    cJustData* m_pGainCorrection;
    cJustData* m_pPhaseCorrection; 
    cJustData* m_pOffsetCorrection;
    
    void Serialize(QDataStream&); // zum schreiben aller justagedaten in flashspeicher
    void Deserialize(QDataStream&); // zum lesen aller justagedaten aus flashspeicher
    
    QString SerializeStatus(); // fürs xml file
    void DeserializeStatus(const QString&);
    void setStatus(int stat);
    int getStatus();
    
private:    
    int m_nStatus;
};


#endif

