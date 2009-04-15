cmake_minimum_required (VERSION 2.6)

project (OpenFlipper)

# add our macro directory to cmake search path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# set (GLUT_ROOT_PATH C:/devel/glut)

include (common)

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

add_subdirectory (OpenMesh/Core)
add_subdirectory (OpenMesh/Tools)
add_subdirectory (ACG)
add_subdirectory (OpenFlipper/PluginLib)
add_subdirectory (OpenFlipper/CoreApp)

add_plugins ()

