cmake_minimum_required (VERSION 2.6)

project (OpenFlipper)

# add our macro directory to cmake search path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# include our cmake files
include (common)

get_version ()

include (package)
include (output)

# check for OpenGL, GLEW and GLUT as our required dependencies
find_package (OpenGL)
if (NOT OPENGL_FOUND)
  message (FATAL_ERROR "OpengGL not found!")
endif ()

find_package (GLEW)
if (NOT GLEW_FOUND)
  message (FATAL_ERROR "GLEW not found!")
endif ()

find_package (GLUT)
if (NOT GLUT_FOUND)
  message (FATAL_ERROR "GLUT not found!")
endif ()

# start with an empty plugin list
of_set (OPENFLIPPER_PLUGINS "")

# add the basic componenets
add_subdirectory (OpenMesh/Core)
add_subdirectory (OpenMesh/Tools)
add_subdirectory (ACG)
add_subdirectory (OpenFlipper/PluginLib)
add_subdirectory (OpenFlipper/CoreApp)

# add plugins
add_plugins ()

if (APPLE)
  # prepare bundle generation cmake file and add a build target for it
  configure_file ("${CMAKE_SOURCE_DIR}/cmake/fixbundle.cmake.in"
     "${CMAKE_BINARY_DIR}/fixbundle.cmake" @ONLY IMMEDIATE)
  add_custom_target (fixbundle ALL
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/fixbundle.cmake"
  )
  # let bundle generation depend on all targets
  add_dependencies (fixbundle OpenFlipper PluginLib ${OPENFLIPPER_PLUGINS})

  # install bundle
  install (DIRECTORY ${CMAKE_BINARY_DIR}/Build/OpenFlipper.app DESTINATION .)
endif ()

# display results
of_print_configure_header ("OpenFlipper")
of_print_plugin_stats ()
of_print_configure_footer ()
