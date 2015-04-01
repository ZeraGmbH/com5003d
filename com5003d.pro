TEMPLATE	= app
LANGUAGE	= C++

include(com5003d.user.pri)

QMAKE_CXXFLAGS += -O0

systemd_notification {
       DEFINES += SYSTEMD_NOTIFICATION
       LIBS +=  -lsystemd
}

LIBS +=  -lSCPI
LIBS +=  -lproto-net-qt
LIBS +=  -lzerai2c
LIBS +=  -lzeramisc
LIBS +=  -lzeraxmlconfig
LIBS +=  -lzeramath
LIBS +=  -lzeradev
LIBS +=  -lprotobuf
LIBS +=  -lzera-resourcemanager-protobuf

CONFIG	+= qt debug

HEADERS	+= \
	zeraglobal.h \
	com5003d.h \
	justdata.h \
    com5003justdata.h \
    com5003scpi.h \
    pcbserver.h \
    i2csettings.h \
    com5003dglobal.h \
    ethsettings.h \
    fpgasettings.h \
    justnode.h \
    scpidelegate.h \
    statusinterface.h \
    scpiconnection.h \
    systeminterface.h \
    atmel.h \
    xmlsettings.h \
    debugsettings.h \
    senseinterface.h \
    sensesettings.h \
    atmelwatcher.h \
    resource.h \
    samplerange.h \
    samplinginterface.h \
    samplingsettings.h \
    senserange.h \
    sensechannel.h \
    sourcesettings.h \
    sourceinterface.h \
    fpzchannel.h \
    adjflash.h \
    adjxml.h \
    adjustment.h \
    systeminfo.h \
    rmconnection.h \
    com5003dprotobufwrapper.h \
    notificationstring.h \
    notificationdata.h \
    protonetcommand.h \
    fpzinchannel.h \
    frqinputinterface.h \
    frqinputsettings.h \
    scheadchannel.h \
    scheadsettings.h \
    scheadinterface.h

SOURCES	+= \
	main.cpp \
	com5003d.cpp \
	justdata.cpp \
    com5003justdata.cpp \
    pcbserver.cpp \
    i2csettings.cpp \
    ethsettings.cpp \
    fpgasettings.cpp \
    justnode.cpp \
    scpidelegate.cpp \
    statusinterface.cpp \
    scpiconnection.cpp \
    systeminterface.cpp \
    atmel.cpp \
    debugsettings.cpp \
    senseinterface.cpp \
    sensesettings.cpp \
    atmelwatcher.cpp \
    samplerange.cpp \
    samplinginterface.cpp \
    samplingsettings.cpp \
    senserange.cpp \
    sensechannel.cpp \
    sourcesettings.cpp \
    fpzchannel.cpp \
    sourceinterface.cpp \
    adjustment.cpp \
    systeminfo.cpp \
    resource.cpp \
    rmconnection.cpp \
    com5003dprotobufwrapper.cpp \
    notificationstring.cpp \
    protonetcommand.cpp \
    fpzinchannel.cpp \
    frqinputinterface.cpp \
    frqinputsettings.cpp \
    scheadchannel.cpp \
    scheadsettings.cpp \
    scheadinterface.cpp

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

target.path = /usr/bin
INSTALLS += target

configxml.path = /etc/zera/com5003d
configxml.files = com5003d.xsd \
                  com5003d.xml

INSTALLS += configxml

QT += xml network

OTHER_FILES +=
