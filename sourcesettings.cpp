#include <QList>
#include <QVariant>

#include <xmlconfigreader.h>

#include "xmlsettings.h"
#include "sourcesettings.h"


cSourceSettings::cSourceSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    SourceSystem::cChannelSettings *settings;
    for (int i = 0; i < 4; i++)
    {
        m_ChannelSettingsList.append(settings = new SourceSystem::cChannelSettings);
        m_ConfigXMLMap[QString("com5003dconfig:resource:source:fpzout:fo%1:name").arg(i)] = SourceSystem::cfg0Name + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:source:fpzout:fo%1:dspserver").arg(i)] = SourceSystem::cfg0dspserver + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:source:fpzout:fo%1:dspchannel").arg(i)] = SourceSystem::cfg0dspchannel + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:source:fpzout:fo%1:avail").arg(i)] = SourceSystem::cfg0avail + i;
    }
}


cSourceSettings::~cSourceSettings()
{
    for (int i = 0; i < m_ChannelSettingsList.count(); i++)
        delete m_ChannelSettingsList.at(i);
}


QList<SourceSystem::cChannelSettings*> &cSourceSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}


void cSourceSettings::configXMLInfo(const QString& key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case SourceSystem::cfg0Name:
            m_ChannelSettingsList.at(0)->m_sName = m_pXMLReader->getValue(key).toString();
            break;
        case SourceSystem::cfg1Name:
            m_ChannelSettingsList.at(1)->m_sName = m_pXMLReader->getValue(key).toString();
            break;
        case SourceSystem::cfg2Name:
            m_ChannelSettingsList.at(2)->m_sName = m_pXMLReader->getValue(key).toString();
            break;
        case SourceSystem::cfg3Name:
            m_ChannelSettingsList.at(3)->m_sName = m_pXMLReader->getValue(key).toString();
            break;
        case SourceSystem::cfg0dspserver:
            m_ChannelSettingsList.at(0)->m_nDspServerPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SourceSystem::cfg1dspserver:
            m_ChannelSettingsList.at(1)->m_nDspServerPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SourceSystem::cfg2dspserver:
            m_ChannelSettingsList.at(2)->m_nDspServerPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SourceSystem::cfg3dspserver:
            m_ChannelSettingsList.at(3)->m_nDspServerPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SourceSystem::cfg0dspchannel:
            m_ChannelSettingsList.at(0)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SourceSystem::cfg1dspchannel:
            m_ChannelSettingsList.at(1)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SourceSystem::cfg2dspchannel:
            m_ChannelSettingsList.at(2)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SourceSystem::cfg3dspchannel:
            m_ChannelSettingsList.at(3)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SourceSystem::cfg0avail:
            m_ChannelSettingsList.at(0)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SourceSystem::cfg1avail:
            m_ChannelSettingsList.at(1)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SourceSystem::cfg2avail:
            m_ChannelSettingsList.at(2)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SourceSystem::cfg3avail:
            m_ChannelSettingsList.at(3)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        }
    }
}

