include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Sphere/Sphere.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypeSphere disabled due to missing Sphere ObjectType" )
endif ()

