#include <QStringList>
#include <scpi.h>
#include <scpiobject.h>
#include <QDebug>

#include "scpidelegate.h"


cSCPIDelegate::cSCPIDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode)
    :cSCPIObject(cmd, type), m_nCmdCode(cmdCode)
{
    qDebug() << cmdParent;
    scpiInterface->genSCPICmd(cmdParent.split(":"), this);
}


bool cSCPIDelegate::executeSCPI(const QString &sInput, QString &sOutput)
{
    emit execute(m_nCmdCode,(QString&) sInput, sOutput);
    return true;
}


