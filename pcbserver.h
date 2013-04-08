/*****************************************************************************/
/**
 * @file pcbserver.h
 * @brief pcbserver.h holds all necessary declarations for the pcb server class
 * @author Peter Lohmer p.lohmer@zera.de
******************************************************************************/

#include <QTcpServer>

#ifndef PCBSERVER_H
#define PCBSERVER_H

/**
  @mainpage base class for pcb servers

  A PCB Server generally has a name and operates on 1 piece of Printed Circuit Board.
  It provides a scpi interface for the resources on the pcb in a generic manner.
  If more than 1 pcb is included in a device the server has to be run for each device.
  A configuration via xml file is possible to configure alternate resources, a server
  can export it's resource configuration to a xml file as valid example.

  It's interface always has a status model to query it's name, the availability of pcb
  and the status of adjustment of the pcb.

  It's resource model allows for querying all available resources.

  It's system model allows querying, setting it's debug level
  */


class cPCBServer: public QTcpServer
{
public:
    cPCBServer();
    /**
      @b reads out the server's name
      */
    QString& getName();
    /**
      @b virtual method for configuring the pcb server, which is called while starting
      the server with additional parameter, which should specify the complete path
      for a valid xml configuration file.
      */
    virtual int configureResource(char* s) = 0; // pure virtual class
    /**
      @b virtual method for exporting a valid configuration xml file, which can be
      used as the starting point for further new configurations.
      */
    virtual exportResourceXML(QString& resFileName) = 0; //

protected:
    /**
      @b virtual method for querying adjustment status of the pcb.
      */
    virtual int getAdjustment() = 0;


private:
    /**
      @b The server's name. The name also specifies the servers service (port number).
      */
    QString m_sServerName;
    /**
      @b A pointer to the server's scpi interface.
      */
    cSCPI *m_pSCPInterface;
    /**
      @b A pointer to the server's client manager, which holds the incoming connections.
      */
    cClientManagement *m_pClientManager;
    /**
      @b The servers debug level, default 0.
      */
    int m_nDebugLevel;

};

#endif // PCBSERVER_H
