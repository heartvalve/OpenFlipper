include (plugin)
if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/SplatCloud/SplatCloud.hh)
  openflipper_plugin ()
else()
  message( WARNING "Plugin FilePTS disabled due to missing SplatCloud ObjectType" )
endif ()

