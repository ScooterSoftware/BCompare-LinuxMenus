set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(KDE4 REQUIRED)

set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${KDE4_KPARTS_DEFINITIONS})
include_directories (${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${KDE4_KPARTS_INCLUDES})

kde4_add_plugin(bcompare_ext_kde bcompare_ext_kde.cpp)
target_link_libraries(bcompare_ext_kde ${KDE4_KIO_LIBS})
install(TARGETS bcompare_ext_kde DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES bcompare_ext_kde.desktop DESTINATION ${SERVICES_INSTALL_DIR})
