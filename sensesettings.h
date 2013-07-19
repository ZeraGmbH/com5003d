#ifndef SENSESETTINGS_H
#define SENSESETTINGS_H

#include <QObject>
#include <QList>

#include "xmlsettings.h"

namespace SenseSystem
{

enum configstate
{
    cfg0Name,
    cfg1Name,
    cfg2Name,
    cfg3Name,
    cfg4Name,
    cfg5Name,
    cfg6Name,
    cfg7Name,
    cfg8Name,
    cfg9Name,
    cfg10Name,
    cfg11Name,
    cfg0ctrlchannel,
    cfg1ctrlchannel,
    cfg2ctrlchannel,
    cfg3ctrlchannel,
    cfg4ctrlchannel,
    cfg5ctrlchannel,
    cfg6ctrlchannel,
    cfg7ctrlchannel,
    cfg8ctrlchannel,
    cfg9ctrlchannel,
    cfg10ctrlchannel,
    cfg11ctrlchannel,
    cfg0dspchannel,
    cfg1dspchannel,
    cfg2dspchannel,
    cfg3dspchannel,
    cfg4dspchannel,
    cfg5dspchannel,
    cfg6dspchannel,
    cfg7dspchannel,
    cfg8dspchannel,
    cfg9dspchannel,
    cfg10dspchannel,
    cfg11dspchannel,
    cfg0avail,
    cfg1avail,
    cfg2avail,
    cfg3avail,
    cfg4avail,
    cfg5avail,
    cfg6avail,
    cfg7avail,
    cfg8avail,
    cfg9avail,
    cfg10avail,
    cfg11avail
};

struct cChannelSettings // what we want to get configured
{
    QString m_sName; // the names channel
    quint8 m_nCtrlChannel; // where to control the channel
    quint8 m_nDspChannel; // where to find the channel's sampled data
    bool avail; // is this channel available ?
};
}


class QString;
namespace Zera
{
namespace XMLConfig
{
    class cReader;
}
}

class cSenseSettings : public cXMLSettings
{
    Q_OBJECT
public:
    cSenseSettings(Zera::XMLConfig::cReader *xmlread);
    virtual ~cSenseSettings();
    QList<SenseSystem::cChannelSettings*>& getChannelSettings();

public slots:
    virtual void configXMLInfo(const QString& key);

private:
    QList<SenseSystem::cChannelSettings*> m_ChannelSettingsList;
};


#endif // SENSESETTINGS_H
