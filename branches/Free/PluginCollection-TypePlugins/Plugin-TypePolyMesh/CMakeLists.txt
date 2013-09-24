include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyMesh/PolyMesh.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypePolyMesh disabled due to missing PolyMesh ObjectType" )
endif ()

