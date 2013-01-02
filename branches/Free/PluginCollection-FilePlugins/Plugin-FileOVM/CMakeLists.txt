include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh)
  openflipper_plugin (DEPS OpenVolumeMesh)
else()
  message( WARNING "Plugin FileOVM disabled due to missing PolyHedralMesh ObjectType" )
endif ()

