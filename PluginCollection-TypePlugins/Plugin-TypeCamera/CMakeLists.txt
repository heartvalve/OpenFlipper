include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Camera/Camera.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypeCamera disabled due to missing Camera ObjectType" )
endif ()