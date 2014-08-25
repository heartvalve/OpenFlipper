include (plugin)

SET( CURRENT_PLUGIN_DEPS "")

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineCurve)
  add_definitions (-DENABLE_SKELETON_SUPPORT)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineSurface)
  add_definitions (-DENABLE_BSPLINESURFACE_SUPPORT)
endif ()

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

openflipper_plugin ( DEPS ${CURRENT_PLUGIN_DEPS}
		     INSTALLDATA Icons )
