include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/SplatCloud/SplatCloud.hh)
  openflipper_plugin (INSTALLDATA Icons Shaders)
else()
  message( WARNING "Plugin TypeSplatCloud disabled due to missing SplatCloud ObjectType" )
endif ()

