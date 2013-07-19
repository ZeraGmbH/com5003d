#ifndef I2CSETTINGS_H
#define I2CSETTINGS_H

#include <QObject>
#include <QString>

#include "xmlsettings.h"


namespace i2cSettings
{
enum member
{
    master,
    atmel,
    flash
};

enum configstate
{
    SetDevNode,
    SetMasterAdr,
    SetAtmelAdr,
    SetFlashAdr
};
}


namespace Zera
{
namespace XMLConfig
{
    class cReader;
}
}


class cI2CSettings : public cXMLSettings
{
    Q_OBJECT

public:
    cI2CSettings(Zera::XMLConfig::cReader *xmlread);
    quint8 getI2CAdress(i2cSettings::member member);
    QString& getDeviceNode();

public slots:
    virtual void configXMLInfo(const QString& key);

private:
    QString m_sDeviceNode;
    quint8 m_nMasterAdr, m_nAtmelAdr, m_nFlashAdr;
};


#endif // I2CSETTINGS_H
