include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Light/Light.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypeLight disabled due to missing Light ObjectType" )
endif ()

