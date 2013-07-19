#ifndef ADJFLASH_H
#define ADJFLASH_H

class QDataStream;
class QString;

class cAdjFlash
{
public:
    virtual void exportAdjData(QDataStream& stream) = 0; // the derived class exports adjdata to qdatastream
    virtual bool importAdjData(QString& s, QDataStream& stream) = 0; // same for import
    virtual quint8 getAdjustmentStatus() = 0;
};

#endif // ADJFLASH_H
