include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/TSplineMesh)
  add_definitions (-DENABLE_TSPLINEMESH_SUPPORT)
endif ()

openflipper_plugin (DIRS TSplineMesh TRANSLATION_LANGUAGES de_DE)
