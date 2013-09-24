include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh)
  openflipper_plugin (DEPS OpenVolumeMesh)
else()
  message( WARNING "Plugin TypePolyhedralMesh disabled due to missing PolyhedralMesh ObjectType" )
endif ()

