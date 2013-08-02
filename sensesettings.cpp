#include <QList>
#include <QVariant>
#include <xmlconfigreader.h>
#include <QDebug>
#include "xmlsettings.h"
#include "sensesettings.h"


cSenseSettings::cSenseSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    SenseSystem::cChannelSettings *settings;
    for (int i = 0; i < 12; i++)
    {
        m_ChannelSettingsList.append(settings = new SenseSystem::cChannelSettings);
        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:ident").arg(i)] = SenseSystem::cfg0Ident + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:ctrlchannel").arg(i)] = SenseSystem::cfg0ctrlchannel + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:dspchannel").arg(i)] = SenseSystem::cfg0dspchannel + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:avail").arg(i)] = SenseSystem::cfg0avail + i;
    }
}


cSenseSettings::~cSenseSettings()
{
    for (int i = 0; i < m_ChannelSettingsList.count(); i++)
        delete m_ChannelSettingsList.at(i);
}


QList<SenseSystem::cChannelSettings*> &cSenseSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}


void cSenseSettings::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case SenseSystem::cfg0Ident:
            m_ChannelSettingsList.at(0)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg1Ident:
            m_ChannelSettingsList.at(1)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg2Ident:
            m_ChannelSettingsList.at(2)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg3Ident:
            m_ChannelSettingsList.at(3)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg4Ident:
            m_ChannelSettingsList.at(4)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg5Ident:
            m_ChannelSettingsList.at(5)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg6Ident:
            m_ChannelSettingsList.at(6)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg7Ident:
            m_ChannelSettingsList.at(7)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg8Ident:
            m_ChannelSettingsList.at(8)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg9Ident:
            m_ChannelSettingsList.at(9)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg10Ident:
            m_ChannelSettingsList.at(10)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg11Ident:
            m_ChannelSettingsList.at(11)->m_sIdent = m_pXMLReader->getValue(key).toString();
            break;
        case SenseSystem::cfg0ctrlchannel:
            m_ChannelSettingsList.at(0)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg1ctrlchannel:
            m_ChannelSettingsList.at(1)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg2ctrlchannel:
            m_ChannelSettingsList.at(2)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg3ctrlchannel:
            m_ChannelSettingsList.at(3)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg4ctrlchannel:
            m_ChannelSettingsList.at(4)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg5ctrlchannel:
            m_ChannelSettingsList.at(5)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg6ctrlchannel:
            m_ChannelSettingsList.at(6)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg7ctrlchannel:
            m_ChannelSettingsList.at(7)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg8ctrlchannel:
            m_ChannelSettingsList.at(8)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg9ctrlchannel:
            m_ChannelSettingsList.at(9)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg10ctrlchannel:
            m_ChannelSettingsList.at(10)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg11ctrlchannel:
            m_ChannelSettingsList.at(11)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg0dspchannel:
            m_ChannelSettingsList.at(0)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg1dspchannel:
            m_ChannelSettingsList.at(1)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg2dspchannel:
            m_ChannelSettingsList.at(2)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg3dspchannel:
            m_ChannelSettingsList.at(3)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg4dspchannel:
            m_ChannelSettingsList.at(4)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg5dspchannel:
            m_ChannelSettingsList.at(5)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg6dspchannel:
            m_ChannelSettingsList.at(6)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg7dspchannel:
            m_ChannelSettingsList.at(7)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg8dspchannel:
            m_ChannelSettingsList.at(8)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg9dspchannel:
            m_ChannelSettingsList.at(9)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg10dspchannel:
            m_ChannelSettingsList.at(10)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg11dspchannel:
            m_ChannelSettingsList.at(11)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg0avail:
            m_ChannelSettingsList.at(0)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg1avail:
            m_ChannelSettingsList.at(1)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg2avail:
            m_ChannelSettingsList.at(2)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg3avail:
            m_ChannelSettingsList.at(3)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg4avail:
            m_ChannelSettingsList.at(4)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg5avail:
            m_ChannelSettingsList.at(5)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg6avail:
            m_ChannelSettingsList.at(6)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg7avail:
            m_ChannelSettingsList.at(7)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg8avail:
            m_ChannelSettingsList.at(8)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg9avail:
            m_ChannelSettingsList.at(9)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg10avail:
            m_ChannelSettingsList.at(10)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        case SenseSystem::cfg11avail:
            m_ChannelSettingsList.at(11)->avail = m_pXMLReader->getValue(key).toBool();
            break;
        }
    }
}
