include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyLine)
  add_definitions (-DENABLE_POLYLINE_SUPPORT)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/TSplineMesh)
  add_definitions (-DENABLE_TSPLINEMESH_SUPPORT)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Skeleton)
  add_definitions (-DENABLE_SKELETON_SUPPORT)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineCurve)
  add_definitions (-DENABLE_BSPLINECURVE_SUPPORT)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineSurface)
  add_definitions (-DENABLE_BSPLINESURFACE_SUPPORT)
endif ()

openflipper_plugin (DIRS PolyLine BSplineCurve BSplineSurface Skeleton TSplineMesh widgets TRANSLATION_LANGUAGES de_DE INSTALLDATA Icons)
