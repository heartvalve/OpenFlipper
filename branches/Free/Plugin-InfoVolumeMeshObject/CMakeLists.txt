include (plugin)


if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyhedralMesh)
    openflipper_plugin (DEPS OpenVolumeMesh )
endif ()
