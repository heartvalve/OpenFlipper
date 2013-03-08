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

openflipper_plugin ( TRANSLATION_LANGUAGES de_DE 
                     INSTALLDATA Icons VsiMetadata  )
