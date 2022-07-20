set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/atmel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/atmelwatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/frqinputinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/hkeyinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/samplinginterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/scheadinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/senseinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/sourceinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/statusinterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/systeminterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/ctrlsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/debugsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/ethsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/fpgasettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/frqinputsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/hkeysettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/i2csettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/samplingsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/scheadsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/sensesettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/sourcesettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/justdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003justdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pcbserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/samplerange.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/senserange.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sensechannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fpzchannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/systeminfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rmconnection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fpzinchannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scheadchannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hkeychannel.cpp
    )

set(HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/atmel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io/atmelwatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/frqinputinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/hkeyinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/samplinginterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/scheadinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/senseinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/sourceinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/statusinterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces/systeminterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/ctrlsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/debugsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/ethsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/fpgasettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/frqinputsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/hkeysettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/i2csettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/samplingsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/scheadsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/sensesettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settings/sourcesettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zeraglobal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/justdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003justdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hkeychannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003scpi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pcbserver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/com5003dglobal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource.h
    ${CMAKE_CURRENT_SOURCE_DIR}/samplerange.h
    ${CMAKE_CURRENT_SOURCE_DIR}/senserange.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sensechannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fpzchannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adjflash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adjxml.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/systeminfo.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rmconnection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/notificationdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fpzinchannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scheadchannel.h
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
    Zera::scpi-tcp
    VeinMeta::xiqnet
    ZeraClasses::zerai2c
    ZeraClasses::zeraproxy 
    ZeraClasses::zeramisc
    ZeraClasses::zeraxmlconfig
    ZeraClasses::zeramath
    ZeraClasses::zeradev
    ZeraClasses::zeraservercommon
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

