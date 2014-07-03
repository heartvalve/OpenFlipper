include(plugin)

SET( CURRENT_PLUGIN_DEPS "")

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyhedralMesh)
    add_definitions (-DENABLE_OPENVOLUMEMESH_SUPPORT)
    add_definitions (-DENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT)

    list( APPEND CURRENT_PLUGIN_DEPS OpenVolumeMesh )
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/HexahedralMesh)
    add_definitions (-DENABLE_OPENVOLUMEMESH_SUPPORT)
    add_definitions (-DENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT)    

    list( APPEND CURRENT_PLUGIN_DEPS OpenVolumeMesh )
endif()

list(REMOVE_DUPLICATES CURRENT_PLUGIN_DEPS )

# use Qwt6 for QT5
if (NOT FORCE_QT4 AND QT5_FOUND)
  find_package(Qwt6)

  if (QWT6_FOUND)
    add_definitions(-DWITH_QWT)
    openflipper_plugin( DEPS ${CURRENT_PLUGIN_DEPS} Qwt6)
  else()
    openflipper_plugin( DEPS ${CURRENT_PLUGIN_DEPS})
  endif()
else()
  find_package(Qwt5)

  if(Qwt5_Qt4_FOUND)
    add_definitions(-DWITH_QWT)
    openflipper_plugin( DEPS ${CURRENT_PLUGIN_DEPS} Qwt5)
  else()
    openflipper_plugin(DEPS ${CURRENT_PLUGIN_DEPS})
  endif()
endif()
