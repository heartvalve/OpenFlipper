include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineCurve)
  add_definitions (-DENABLE_SKELETON_SUPPORT)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyhedralMesh)
    add_definitions (-DENABLE_OPENVOLUMEMESH_SUPPORT)
endif()

openflipper_plugin (INSTALLDATA Icons DIRS OpenMesh OpenVolumeMesh Widgets)
