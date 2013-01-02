include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Sphere/Sphere.hh)
  openflipper_plugin ()
else()
  message( WARNING "Plugin FileSPH disabled due to missing Sphere ObjectType" )
endif ()
