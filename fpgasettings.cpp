#include <QVariant>
#include <xmlconfigreader.h>

#include "com5003dglobal.h"
#include "fpgasettings.h"


cFPGASettings::cFPGASettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["connectivity:fpga:device:node"] = FPGASettings::setFPGADevNode;
    m_sDeviceNode = defaultFPGADeviceNode;
}


QString& cFPGASettings::getDeviceNode()
{
    return m_sDeviceNode;
}


void cFPGASettings::configXMLInfo(const QString& key)
{
    if (m_ConfigXMLMap.contains(key))
    {
        m_sDeviceNode = m_pXMLReader->getValue(key).toString();
    }
}
