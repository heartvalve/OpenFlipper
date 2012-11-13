include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineCurve)
  add_definitions (-DENABLE_BSPLINECURVE_SUPPORT)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineSurface)
  add_definitions (-DENABLE_BSPLINESURFACE_SUPPORT)
endif ()

openflipper_plugin ()
