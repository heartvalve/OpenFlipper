include (plugin)

SET( CURRENT_PLUGIN_DEPS "")

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyLine)
  add_definitions (-DENABLE_POLYLINE_SUPPORT)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/TSplineMesh)
  add_definitions (-DENABLE_TSPLINEMESH_SUPPORT)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Skeleton)
  add_definitions (-DENABLE_SKELETON_SUPPORT)
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
		     TRANSLATION_LANGUAGES de_DE
                     INSTALLDATA Icons VsiMetadata )


