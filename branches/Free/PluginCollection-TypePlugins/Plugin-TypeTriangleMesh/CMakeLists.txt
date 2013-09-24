include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/TriangleMesh/TriangleMesh.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypeTriangleMesh disabled due to missing TriangleMesh ObjectType" )
endif ()
