
# Configure the KDE version to build on
option(TARGET_KDE6 "Target KDE6 instead of KDE5" ON)
if(TARGET_KDE6)
    cmake_minimum_required(VERSION 3.16)
    set(QT_MAJOR_VERSION 6)
    set(QT_MIN_VERSION "6.6.0")
    set(KF_MIN_VERSION "6.0.0")
else()
    # Support at least Debian 10 buster
    cmake_minimum_required(VERSION 3.13)
    set(QT_MAJOR_VERSION 5)
    set(QT_MIN_VERSION "5.11.0")
    set(KF_MIN_VERSION "5.54.0")
endif()

project (bcompare_ext_kde)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets)
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS KIO I18n CoreAddons)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

# Check if KIO has CommandLauncherJob class
set(HAVE_COMMAND_LAUNCHER_JOB ON)
if(NOT TARGET_KDE6)
    if(${KF5KIO_VERSION} VERSION_LESS "5.69.0")
        set(HAVE_COMMAND_LAUNCHER_JOB OFF)
    endif()
endif()

if(NOT HAVE_COMMAND_LAUNCHER_JOB)
    add_definitions(-DDO_NOT_HAVE_COMMAND_LAUNCHER_JOB=1)
endif()

add_definitions(-DQT_NO_CAST_TO_ASCII=1)
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY=1)
add_definitions(-DQT_NO_CAST_FROM_ASCII=1)
add_definitions(-DTRANSLATION_DOMAIN="bcompare_ext_kde")

if(TARGET_KDE6)
    qt6_add_resources(bcompare_ext_kde_QRC bcompare_icon.qrc OPTIONS -no-compress)
else()
    qt5_add_resources(bcompare_ext_kde_QRC bcompare_icon.qrc OPTIONS -no-compress)
endif()

set(bcompare_ext_kde_SRCS
    bcompare_ext_kde.cpp
    bcompare_config.cpp
    ${bcompare_ext_kde_QRC})

kcoreaddons_add_plugin(bcompare_ext_kde
                       SOURCES ${bcompare_ext_kde_SRCS}
                       INSTALL_NAMESPACE "kf${QT_MAJOR_VERSION}/kfileitemaction")

target_link_libraries(bcompare_ext_kde KF${QT_MAJOR_VERSION}::KIOWidgets
                                       KF${QT_MAJOR_VERSION}::KIOGui KF${QT_MAJOR_VERSION}::I18n)

ki18n_install(po)
