#include <xmlconfigreader.h>
#include <QVariant>

#include "samplingsettings.h"

cSamplingSettings::cSamplingSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    SamplingSystem::cChannelSettings* settings = new SamplingSystem::cChannelSettings;
    m_ChannelSettingsList.append(settings);

    m_ConfigXMLMap["resource:sample:0:name"] = SamplingSystem::cfgName;
    m_ConfigXMLMap["resource:sample:0:avail"] = SamplingSystem::cfgAvail;
}


cSamplingSettings::~cSamplingSettings()
{
    for (int i = 0; i < m_ChannelSettingsList.count(); i++)
        delete m_ChannelSettingsList.at(i);
}


QList<SamplingSystem::cChannelSettings *> &cSamplingSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}


void cSamplingSettings::configXMLInfo(const QString& key)
{
    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case SamplingSystem::cfgName:
            m_ChannelSettingsList.at(0)->m_sName = m_pXMLReader->getValue(key).toString();
            break;
        case SamplingSystem::cfgAvail:
            m_ChannelSettingsList.at(0)->m_bAvail = m_pXMLReader->getValue(key).toBool();
            break;
        }
    }
}
