#include <QList>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include <QDomText>
#include <QDebug>
#include "xmlsettings.h"
#include "scpiconnection.h"
#include "resource.h"

#include "justdata.h"
#include "sensesettings.h"
#include "senseinterface.h"
#include "sensechannel.h"
#include "senserange.h"
#include "atmel.h"
#include "adjflash.h"


cSenseInterface::cSenseInterface(cSenseSettings *senseSettings)
{
    int i;
    QList<SenseSystem::cChannelSettings*> mySettings;

    mySettings = senseSettings->getChannelSettings();

    // our sense has 3 voltage and 3 current measuring channels
    cSenseChannel* pChannel;
    pChannel = new cSenseChannel("Measuring channel 0..480V AC","V", mySettings.at(0), 0);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Measuring channel 0..480V AC","V", mySettings.at(1), 1);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Measuring channel 0..480V AC","V", mySettings.at(2), 2);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Measuring channel 0..160A AC","A", mySettings.at(3), 3);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Measuring channel 0..160A AC","A", mySettings.at(4), 4);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Measuring channel 0..160A AC","A", mySettings.at(5), 5);
    m_ChannelList.append(pChannel);

    // and 6 reference channels
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", mySettings.at(6), 6);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", mySettings.at(7), 7);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", mySettings.at(8), 8);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", mySettings.at(9), 9);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", mySettings.at(10), 10);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", mySettings.at(11), 11);
    m_ChannelList.append(pChannel);

    QList<cSenseRange*> rngList;

    for (i = 0; i < 3; i++)
    {
        rngList.clear();
        rngList.append(new cSenseRange("480V",480.0,4712563.0, 0, SenseRange::Phys));
        rngList.append(new cSenseRange("240V",240.0,4712563.0, 1, SenseRange::Phys));
        rngList.append(new cSenseRange("120V",120.0,4712563.0, 2, SenseRange::Phys));
        rngList.append(new cSenseRange("60V",60.0,4859831.0, 3, SenseRange::Phys));
        rngList.append(new cSenseRange("15V",15.0,4712563.0, 4, SenseRange::Phys));
        rngList.append(new cSenseRange("5V",5.0,4712563.0, 5, SenseRange::Phys));

        m_ChannelList.at(i)->setRangeList(rngList);
    }

    for (i = 3; i < 6; i++)
    {
        rngList.clear();
        rngList.append(new cSenseRange("160A",160.0,5005789.0, 0, SenseRange::Phys));
        rngList.append(new cSenseRange("100A",100.0,4692928.0, 1, SenseRange::Phys));
        rngList.append(new cSenseRange("50A",100.0,4692928.0, 2, SenseRange::Phys));
        rngList.append(new cSenseRange("25A",25.0,4692928.0, 3, SenseRange::Phys));
        rngList.append(new cSenseRange("10A",10.0,4692928.0, 4, SenseRange::Phys));
        rngList.append(new cSenseRange("2.5A",2.5,4692928.0, 5, SenseRange::Phys));
        rngList.append(new cSenseRange("1.0A",1.0,4692928.0, 6, SenseRange::Phys));
        rngList.append(new cSenseRange("500mA",0.5,4692928.0, 7, SenseRange::Phys));
        rngList.append(new cSenseRange("250mA",0.25,4692928.0, 8, SenseRange::Phys));
        rngList.append(new cSenseRange("100mA",0.1,4692928.0, 9, SenseRange::Phys));
        rngList.append(new cSenseRange("50mA",0.05,4692928.0, 10, SenseRange::Phys));
        rngList.append(new cSenseRange("25mA",0.025,4692928.0, 11, SenseRange::Phys));
        rngList.append(new cSenseRange("10mA",0.01,4692928.0, 12, SenseRange::Phys));
        rngList.append(new cSenseRange("5mA",0.005,4692928.0, 13, SenseRange::Phys));

        m_ChannelList.at(i)->setRangeList(rngList);
    }

    for (i = 6; i < 12; i++)
    {
        rngList.clear();
        rngList.append(new cSenseRange("0V",10.0,5005789.0, 14, SenseRange::Phys));
        rngList.append(new cSenseRange("10V",10.0,5005789.0, 15, SenseRange::Phys));

        m_ChannelList.at(i)->setRangeList(rngList);
    }

    m_sVersion = SenseSystem::Version;
}


cSenseInterface::~cSenseInterface()
{
    int i;
    cSenseChannel* cptr;

    for (i = 0; i < m_ChannelList.count(); i++)
    {
        cptr = m_ChannelList.at(i);
        delete cptr;
    }
}


void cSenseInterface::initSCPIConnection(QString leadingNodes, cSCPI* scpiInterface)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1SENSE").arg(leadingNodes),"VERSION",SCPI::isQuery,scpiInterface, SenseSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));
    delegate = new cSCPIDelegate(QString("%1SENSE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, scpiInterface, SenseSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int,QString&,QString&)), this, SLOT(executeCommand(int,QString&,QString&)));

    for (int i = 0; i < m_ChannelList.count(); i++)
        m_ChannelList.at(i)->initSCPIConnection(QString("%1SENSE").arg(leadingNodes),scpiInterface);
}


cSenseChannel *cSenseInterface::getChannel(QString &name)
{
    int i;

    for (i = 0; i < m_ChannelList.count(); i++)
        if (m_ChannelList.at(i)->getName() == name)
            break;

    if (i < m_ChannelList.count())
        return m_ChannelList.at(i);
    else
        return 0;
}


quint8 cSenseInterface::getAdjustmentStatus()
{
    quint8 adj = 255;
    for (int i = 0; i < m_ChannelList.count(); i++)
        adj &= m_ChannelList.at(i)->getAdjustmentStatus();

    return adj;
}


void cSenseInterface::registerResource(QDataStream &stream)
{

}


void cSenseInterface::executeCommand(int cmdCode, QString &sInput, QString &sOutput)
{
    switch (cmdCode)
    {
    case SenseSystem::cmdVersion:
        sOutput = m_ReadVersion(sInput);
        break;
    case SenseSystem::cmdChannelCat:
        sOutput = m_ReadSenseChannelCatalog(sInput);
        break;
    }
}


bool cSenseInterface::importAdjData(QString &s, QDataStream &stream)
{
    QStringList spec;
    spec = s.split(':');
    if (spec.at(0) == "SENSE" )
    {
        cSenseChannel* chn;
        QString s = spec.at(1);
        if ((chn = getChannel(s)) != 0)
        {
            cSenseRange* rng;
            s = spec.at(2);
            if ((rng = chn->getRange(s)) != 0)
            {
                rng->getJustData()->Deserialize(stream);
                return true;
            }
        }
        cCOM5003JustData dummy; // if the data was for SENSE but we didn't find channel or range
        dummy.Deserialize(stream); // we read the data from stream to keep it in flow
        return true;
    }
    else
        return false;
}


void cSenseInterface::exportAdjData(QDataStream &stream)
{
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        QList<cSenseRange*> list = m_ChannelList.at(i)->getRangeList();
        QString spec;

        for (int j = 0; j < list.count(); j ++)
        {
             spec = QString("%1:%2:%3")
                 .arg("SENSE")
                 .arg(m_ChannelList.at(i)->getName())
                 .arg(list.at(j)->getName());

             stream << spec.toLatin1();
             list.at(j)->getJustData()->Serialize(stream);
        }
    }
}


void cSenseInterface::exportAdjData(QDomDocument& doc, QDomElement& qde)
{
    QDomElement typeTag = doc.createElement( "Sense");
    qde.appendChild(typeTag);

    for (int i = 0; i < m_ChannelList.count(); i ++)
    {
        QDomText t;
        QDomElement chtag = doc.createElement( "Channel" );
        typeTag.appendChild( chtag );
        QDomElement nametag = doc.createElement( "Name" );
        chtag.appendChild(nametag);
        t = doc.createTextNode(m_ChannelList.at(i)->getName());
        nametag.appendChild( t );

        QList<cSenseRange*> list = m_ChannelList.at(i)->getRangeList();
        for (int j = 0; j < list.count(); j++)
        {
            cSenseRange* rng = list.at(j);

            QDomElement rtag = doc.createElement( "Range" );
            chtag.appendChild( rtag );

            nametag = doc.createElement( "Name" );
            rtag.appendChild(nametag);

            t = doc.createTextNode(list.at(j)->getName());
            nametag.appendChild( t );

            QDomElement gpotag = doc.createElement( "Gain" );
            rtag.appendChild(gpotag);
            QDomElement tag = doc.createElement( "Status" );
            QString jdata = rng->getJustData()->m_pGainCorrection->SerializeStatus();
            t = doc.createTextNode(jdata);
            gpotag.appendChild(tag);
            tag.appendChild(t);
            tag = doc.createElement( "Coefficients" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pGainCorrection->SerializeCoefficients();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
            tag = doc.createElement( "Nodes" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pGainCorrection->SerializeNodes();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);

            gpotag = doc.createElement( "Phase" );
            rtag.appendChild(gpotag);
            tag = doc.createElement( "Status" );
            jdata = rng->getJustData()->m_pPhaseCorrection->SerializeStatus();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
            gpotag.appendChild(tag);
            tag = doc.createElement( "Coefficients" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pPhaseCorrection->SerializeCoefficients();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
            tag = doc.createElement( "Nodes" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pPhaseCorrection->SerializeNodes();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);

            gpotag = doc.createElement( "Offset" );
            rtag.appendChild(gpotag);
            tag = doc.createElement( "Coefficients" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pOffsetCorrection->SerializeCoefficients();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
            tag = doc.createElement( "Nodes" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pOffsetCorrection->SerializeNodes();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
        }
    }
}


bool cSenseInterface::importAdjData(QDomNode& node) // n steht auf einem element dessen tagname channel ist
{
    qDebug() << node.toElement().tagName();
    if (node.toElement().tagName() != "Sense") // data not for us
        return false;

    QDomNodeList nl=node.childNodes(); // we have a list our channels entries now

    for (quint32 i = 0; i < nl.length(); i++)
    {
        QDomNode chnNode = nl.item(i); // we iterate over all channels from xml file

        QDomNodeList nl2 = chnNode.childNodes();
        for (quint32 j = 0; j < nl2.length(); j++)
        {
            cSenseChannel* chnPtr;
            cSenseRange* rngPtr;
            QString Name;

            QDomNode ChannelJustNode = nl2.item(j);
            QDomElement e=ChannelJustNode.toElement();
            QString tName=e.tagName();
            qDebug() << tName;

            if (tName == "Name")
            {
                Name=e.text();
                qDebug() << Name;
                chnPtr = getChannel(Name);
            }

            if (tName == "Range")
            {
                if (chnPtr != 0) // if we know this channel
                {
                    QDomNodeList nl3 = ChannelJustNode.childNodes();

                    for (quint32 k = 0; k < nl3.length(); k++)
                    {
                        QDomNode RangeJustNode = nl3.item(k);

                        e = RangeJustNode.toElement();
                        tName = e.tagName();
                        qDebug() << tName;

                        if (tName == "Name")
                        {
                            Name=e.text();
                            qDebug() << Name;
                            rngPtr = chnPtr->getRange(Name);
                        }

                        cJustData* pJustData = 0;

                        if (rngPtr != 0)
                        {
                            if (tName == "Gain")
                                pJustData = rngPtr->getJustData()->m_pGainCorrection;

                            if (tName == "Phase")
                                pJustData = rngPtr->getJustData()->m_pPhaseCorrection;

                            if (tName == "Offset")
                                pJustData = rngPtr->getJustData()->m_pOffsetCorrection;
                        }

                        if (pJustData)
                        {
                            QDomNodeList nl4 = RangeJustNode.childNodes();
                            for (qint32 k = 0; k < nl4.count(); k++)
                            {
                                QDomNode jTypeNode = nl4.item(k);
                                QString jTypeName = jTypeNode.toElement().tagName();
                                QString jdata = jTypeNode.toElement().text();

                                if (jTypeName == "Status")
                                    pJustData->DeserializeStatus(jdata);

                                if (jTypeName == "Coefficients")
                                    pJustData->DeserializeCoefficients(jdata);

                                if (jTypeName == "Nodes")
                                    pJustData->DeserializeCoefficients(jdata);
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}


QString cSenseInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion+";";
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}


QString cSenseInterface::m_ReadSenseChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s;
        for (int i = 0; i < m_ChannelList.count(); i++ )
            s += m_ChannelList.at(i)->getName() + ";";
        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak]+";";
}


/*
void cSenseInterface::registerResource(QDataStream &stream)
{
    for (int i = 0; i < m_ChannelList.count(); i++)
        if (m_ChannelList.at(i)->isAvail() )
            stream << QString("RESOURCE:ADD SENSE;%1;;%2").arg(m_ChannelList.at(i)->getName()
                                                               .arg((m_ChannelList.at(i)->getDescription());
}
*/






