include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyLine)
  add_definitions (-DENABLE_POLYLINE_SUPPORT)
endif ()

openflipper_plugin (DIRS PolyLine BSplineCurve widgets TRANSLATION_LANGUAGES de_DE)