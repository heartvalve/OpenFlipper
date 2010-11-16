include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Skeleton)
  add_definitions (-DENABLE_SKELETON_SUPPORT)
endif ()

openflipper_plugin ()
