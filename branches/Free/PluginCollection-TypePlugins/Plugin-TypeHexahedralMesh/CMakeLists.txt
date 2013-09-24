include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/HexahedralMesh/HexahedralMesh.hh)
  openflipper_plugin (DEPS OpenVolumeMesh)
else()
  message( WARNING "Plugin TypeHexahedralMesh disabled due to missing HexahedralMesh ObjectType" )
endif ()
