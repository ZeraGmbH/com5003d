cmake_minimum_required(VERSION 3.1)

project(com5003d LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(com5003d_VERSION_MAJOR "1")
set(com5003d_VERSION_MINOR "1")
set(com5003d_VERSION_PATCH "0")

set(PROJECT_VERSION "${com5003d_VERSION_MAJOR}.${com5003d_VERSION_MINOR}.${com5003d_VERSION_PATCH}")

include(FeatureSummary)
include(GNUInstallDirs)

option(useSystemd "Using libsystemd" ON)

find_package(Qt5 COMPONENTS Core Gui Xml Network CONFIG REQUIRED)
find_package(SCPI REQUIRED)
find_package(scpi-tcp REQUIRED)
find_package(xiqnet REQUIRED)
find_package(zerai2c REQUIRED)
find_package(zeraproxy REQUIRED)
find_package(zeramisc REQUIRED)
find_package(zeraxmlconfig REQUIRED)
find_package(zeramath REQUIRED)
find_package(zeradev REQUIRED)
find_package(zeraservercommon REQUIRED)
find_package(VfProtobuf REQUIRED)
find_package(zeraprotobuf REQUIRED)

if(${useSystemd})
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Systemd REQUIRED libsystemd)
    add_definitions(-DSYSTEMD_NOTIFICATION)
endif()

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
