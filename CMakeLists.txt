cmake_minimum_required (VERSION 2.6)

project (OpenFlipper)

# add our macro directory to cmake search path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set (GLUT_ROOT_PATH C:/devel/glut)

include (common)
include (package)

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

of_set (OPENFLIPPER_PLUGINS "")

add_subdirectory (OpenMesh/Core)
add_subdirectory (OpenMesh/Tools)
add_subdirectory (ACG)
add_subdirectory (OpenFlipper/PluginLib)
add_subdirectory (OpenFlipper/CoreApp)

add_plugins ()

if (APPLE)
  configure_file ("${CMAKE_SOURCE_DIR}/cmake/fixbundle.cmake.in"
     "${CMAKE_BINARY_DIR}/fixbundle.cmake" @ONLY IMMEDIATE)
  add_custom_target (fixbundle ALL
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/fixbundle.cmake"
  )
  add_dependencies (fixbundle OpenFlipper PluginLib ${OPENFLIPPER_PLUGINS})

  # install
  install (DIRECTORY ${CMAKE_BINARY_DIR}/Build/OpenFlipper.app DESTINATION .)
endif ()
