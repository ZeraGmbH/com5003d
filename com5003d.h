// header datei com5003.h
// globale definitionen

#ifndef COM5003D_H
#define COM5003D_H

#include "pcbserver.h"

class QStateMachine;
class QState;
class cDebugSettings;
class cFPGASettings;
class cI2CSettings;
class cETHSettings;
class cSenseSettings;
class cSamplingSettings;
class cSourceSettings;
class cStatusInterface;
class cSystemInterface;
class cSenseInterface;
class cSamplingInterface;
class cSourceInterface;
class cSystemInfo;
class cAdjustment;
class cAtmelWatcher;
class cRMConnection;


class cCOM5003dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cCOM5003dServer(QObject* parent=0);
    ~cCOM5003dServer();

    cDebugSettings* m_pDebugSettings;
    cFPGASettings* m_pFPGAsettings;
    cI2CSettings* m_pI2CSettings;
    cETHSettings* m_pETHSettings;
    cSenseSettings* m_pSenseSettings;
    cSamplingSettings* m_pSamplingSettings;
    cSourceSettings* m_pSourceSettings;

    cStatusInterface* m_pStatusInterface;
    cSystemInterface* m_pSystemInterface;
    cSenseInterface* m_pSenseInterface;
    cSamplingInterface* m_pSamplingInterface;
    cSourceInterface* m_pSourceInterface;
    cSystemInfo* m_pSystemInfo;
    cAdjustment* m_pAdjHandler;
    cRMConnection* m_pRMConnection;


signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void atmelRunning();
    void serverSetup();

private:
    QStateMachine* m_pInitializationMachine;
    QState* stateconnect2RM;
    QState* stateSendRMIdentandRegister;
    cAtmelWatcher* m_pAtmelWatcher;
    quint8 m_nerror;
    int m_nFPGAfd;

private slots:
    void doConfiguration();
    void doWait4Atmel();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void doIdentAndRegister();
};


#endif
