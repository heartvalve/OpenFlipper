include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyLine)
  add_definitions (-DENABLE_POLYLINE_SUPPORT)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/TSplineMesh)
  add_definitions (-DENABLE_TSPLINEMESH_SUPPORT)
endif ()

openflipper_plugin (DIRS PolyLine BSplineCurve TSplineMesh widgets TRANSLATION_LANGUAGES de_DE)
