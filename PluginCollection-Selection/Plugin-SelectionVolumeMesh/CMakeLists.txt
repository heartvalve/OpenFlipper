include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/HexahedralMesh)
  openflipper_plugin (DEPS OpenVolumeMesh
                      INSTALLDATA Icons)
endif()
