include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Skeleton/Skeleton.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypeSkeleton disabled due to missing Skeleton ObjectType" )
endif ()
