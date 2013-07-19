/*****************************************************************************/
/**
 * @file pcbserver.h
 * @brief pcbserver.h holds all necessary declarations for the pcb server class
 * @author Peter Lohmer p.lohmer@zera.de
******************************************************************************/
#ifndef PCBSERVER_H
#define PCBSERVER_H

#include <QObject>
#include <QList>
#include "scpiconnection.h"

class QTcpSocket;
class QByteArray;
class cResource;

namespace Zera
{
    namespace Net
    {
        class cServer;
        class cClient;
    }
    namespace XMLConfig
    {
        class cReader;
    }
}
class cSCPI;
class cStatusInterface;  // forward


/**
  @mainpage base class for pcb servers

  A PCB Server generally has a name and operates on 1 piece of Printed Circuit Board.
  It provides a scpi interface for the resources on the pcb in a generic manner.
  If more than 1 pcb is included in a device, the server has to be run for each device.
  A configuration via xml file is necessary to configure alternate resources, a server
  can export it's resource configuration to a xml file as a template.

  It's interface always has a status model to query it's name, the availability of pcb
  and the status of adjustment of the pcb and it provides a system model to query version
  information and to provide update facilities.

  It's resource model allows for querying all available resources.

  */


class cPCBServer: public QObject
{
    Q_OBJECT

public:
    /**
      @b Initialise the const variables and connections for new clients and their commands
      @param the servers name
      */
    cPCBServer();
    /**
      @b reads out the server's name
      */
    QString& getName();
    QString& getVersion();

    cStatusInterface* m_pStatusInterface;


signals:
    void sendAnswer(QByteArray answer);


protected:
    void initSCPIConnections();
    void registerResources();
    Zera::Net::cServer* myServer; // the real server that does the communication job
    Zera::XMLConfig::cReader* myXMLConfigReader; // the xml configurator
    QString m_sConfigurationPath;
    QList<cSCPIConnection*> scpiConnectionList; // a list of all scpi connections
    QList<cResource*> resourceList;

protected slots:
    virtual void doConfiguration() = 0; // all servers must configure
    virtual void setupServer(); // all servers must setup

private:
    /**
      @b The server's name. The name also specifies the servers service (port number).
      */
    QString m_sServerName;
    QString m_sServerVersion;

    /**
      @b A pointer to the server's scpi interface.
      */
    cSCPI *m_pSCPInterface;
    QString m_sInput, m_sOutput, m_sParam;
    QTcpSocket* resourceManagerSocket;

private slots:
    virtual void establishNewConnection(Zera::Net::cClient* newClient);
    virtual void executeCommand(const QByteArray cmd);

};

#endif // PCBSERVER_H
