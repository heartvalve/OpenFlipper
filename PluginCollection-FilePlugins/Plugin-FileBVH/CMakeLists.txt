include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Skeleton/Skeleton.hh)
  openflipper_plugin ()
else()
  message( WARNING "Plugin FileBVH disabled due to missing skeleton ObjectType" )
endif ()

