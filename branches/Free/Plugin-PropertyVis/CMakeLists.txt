include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineCurve)
  add_definitions (-DENABLE_SKELETON_SUPPORT)
endif()

openflipper_plugin (INSTALLDATA Icons)
