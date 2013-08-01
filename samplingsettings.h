#ifndef SAMPLINGSETTINGS_H
#define SAMPLINGSETTINGS_H

#include <QList>
#include "xmlsettings.h"

namespace SamplingSystem
{

enum configstate
{
    cfgIdent,
    cfgAvail
};


struct cChannelSettings // what we want to get configured
{
    QString m_sIdent; // the names channel
    bool m_bAvail; // is this channel available ?
};

} // namespace SamplingSystem


class cSamplingSettings : public cXMLSettings
{
public:
    cSamplingSettings(Zera::XMLConfig::cReader *xmlread);
    ~cSamplingSettings();

    QList<SamplingSystem::cChannelSettings*>& getChannelSettings();

public slots:
    virtual void configXMLInfo(QString key);

private:
    QList<SamplingSystem::cChannelSettings*> m_ChannelSettingsList;
};



#endif // SAMPLINGSETTINGS_H
