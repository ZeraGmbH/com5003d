#include <QVariant>
#include <xmlconfigreader.h>

#include "com5003dglobal.h"
#include "ethsettings.h"


cETHSettings::cETHSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["com5003dconfig:connectivity:ethernet:port:server"] = setServerPort;
    m_ConfigXMLMap["com5003dconfig:connectivity:ethernet:port:resourcemanager"] = setRMPort;
    m_nServerPort = defaultServerPort;
    m_nRMPort = defaultRMPort;
}


quint16 cETHSettings::getPort(ethmember member)
{
    quint16 port;

    switch (member)
    {
    case server:
        port = m_nServerPort;
        break;
    case resourcemanager:
        port = m_nRMPort;
        break;
    }

    return port;
}


void cETHSettings::configXMLInfo(const QString& key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case setServerPort:
            m_nServerPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMPort:
            m_nRMPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        }
    }
}
