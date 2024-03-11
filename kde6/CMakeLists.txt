
cmake_minimum_required(VERSION 3.16)
project (bcompare_ext_kde6)

set(QT_MAJOR_VERSION 6)
set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.0.0")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS KIO I18n CoreAddons)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_definitions(-DQT_NO_CAST_TO_ASCII=1)
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY=1)
add_definitions(-DQT_NO_CAST_FROM_ASCII=1)
add_definitions(-DTRANSLATION_DOMAIN="bcompare_ext_kde6")

qt6_add_resources(bcompare_ext_kde6_QRC bcompare_icon.qrc OPTIONS -no-compress)

set(bcompare_ext_kde6_SRCS
    bcompare_ext_kde6.cpp
    bcompare_config.cpp
    ${bcompare_ext_kde6_QRC})

kcoreaddons_add_plugin(bcompare_ext_kde6
                       SOURCES ${bcompare_ext_kde6_SRCS}
                       INSTALL_NAMESPACE "kf6/kfileitemaction")

target_link_libraries(bcompare_ext_kde6 KF6::KIOWidgets KF6::KIOGui KF6::I18n)

ki18n_install(po)
