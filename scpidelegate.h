#ifndef SCPIDELEGATE_H
#define SCPIDELEGATE_H

#include <QObject>

#include "scpiobject.h"

class QString;
class cSCPI;


class cSCPIDelegate: public QObject, public cSCPIObject
{
   Q_OBJECT

public:
    cSCPIDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode);
    virtual bool executeSCPI(const QString& sInput, QString& sOutput);

signals:
    void execute(int cmdCode, QString& sInput, QString& sOutput);

private:
    quint16 m_nCmdCode;
};


#endif // SCPIDELEGATE_H
