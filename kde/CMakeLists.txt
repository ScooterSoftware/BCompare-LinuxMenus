
cmake_minimum_required(VERSION 3.3)
project (bcompare_ext_kde)

set(QT_MAJOR_VERSION 5)
set(QT_MIN_VERSION "5.11.0")
set(KF5_MIN_VERSION "5.54.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS KIO I18n CoreAddons)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_definitions(-DQT_NO_CAST_TO_ASCII=1)
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY=1)
add_definitions(-DQT_NO_CAST_FROM_ASCII=1)
add_definitions(-DTRANSLATION_DOMAIN="bcompare_ext_kde")

qt5_add_resources(bcompare_ext_kde_QRC bcompare_icon.qrc OPTIONS -no-compress)

set(bcompare_ext_kde_SRCS
    bcompare_ext_kde.cpp
    bcompare_config.cpp
    ${bcompare_ext_kde_QRC})

kcoreaddons_add_plugin(bcompare_ext_kde
                       SOURCES ${bcompare_ext_kde_SRCS}
                       JSON "${CMAKE_CURRENT_BINARY_DIR}/bcompare_ext_kde.json"
                       INSTALL_NAMESPACE "kf5/kfileitemaction")

target_link_libraries(bcompare_ext_kde KF5::KIOWidgets KF5::I18n)

ki18n_install(po)
