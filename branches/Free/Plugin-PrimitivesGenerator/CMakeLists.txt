include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineCurve)
  add_definitions (-DENABLE_SKELETON_SUPPORT)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyhedralMesh)
    add_definitions (-DENABLE_OPENVOLUMEMESH_SUPPORT)
    add_definitions (-DENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/HexahedralMesh)
    add_definitions (-DENABLE_OPENVOLUMEMESH_SUPPORT)
    add_definitions (-DENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT)
endif()

openflipper_plugin ( INSTALLDATA Icons )
