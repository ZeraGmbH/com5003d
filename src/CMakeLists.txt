set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/atmel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/atmelwatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/justdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003justdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pcbserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/i2csettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ethsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fpgasettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/justnode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpidelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/statusinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpiconnection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/systeminterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debugsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/senseinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sensesettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/samplerange.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/samplinginterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/samplingsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/senserange.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sensechannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sourcesettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fpzchannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sourceinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/systeminfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rmconnection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/notificationstring.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/protonetcommand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fpzinchannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frqinputinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frqinputsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scheadchannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scheadsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scheadinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hkeysettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hkeyinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hkeychannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hkeychannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hkeyinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hkeysettings.h
    )

set(HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/atmel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/atmelwatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zeraglobal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/justdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003justdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003scpi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pcbserver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/i2csettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003dglobal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ethsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fpgasettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/justnode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpidelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/statusinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpiconnection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/systeminterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/xmlsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/debugsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/senseinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sensesettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource.h
    ${CMAKE_CURRENT_SOURCE_DIR}/samplerange.h
    ${CMAKE_CURRENT_SOURCE_DIR}/samplinginterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/samplingsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/senserange.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sensechannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sourcesettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sourceinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fpzchannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adjflash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adjxml.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/systeminfo.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rmconnection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/notificationstring.h
    ${CMAKE_CURRENT_SOURCE_DIR}/notificationdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/protonetcommand.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fpzinchannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/frqinputinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/frqinputsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scheadchannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scheadsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scheadinterface.h
    )

set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003d.xml
    )
set(CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003d.xsd
    )

# check for valid config files / display in qt-creator
add_custom_target(com5003-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

add_executable(com5003d
    ${SOURCES}
    ${HEADER}
    )

if(${Systemd_FOUND})
    target_link_libraries(com5003d
        PRIVATE
        systemd
    )
endif()

target_link_libraries(com5003d
    PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Xml
    Qt5::Network
    Zera::SCPI
    VeinMeta::xiqnet
    ZeraClasses::zerai2c
    ZeraClasses::zeraproxy 
    ZeraClasses::zeramisc
    ZeraClasses::zeraxmlconfig
    ZeraClasses::zeramath
    ZeraClasses::zeradev
    VeinMeta::VfProtobuf
    VeinMeta::zeraprotobuf
    )	

# announce headers
target_include_directories(com5003d
    PUBLIC
    $<BUILD_INTERFACE:${MY_LIBRARY_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/com5003d>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

install(TARGETS com5003d
    EXPORT com5003dExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/com5003d
    )

