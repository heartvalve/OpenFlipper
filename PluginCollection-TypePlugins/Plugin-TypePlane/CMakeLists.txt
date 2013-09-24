include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Plane/Plane.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypePlane disabled due to missing Plane ObjectType" )
endif ()

