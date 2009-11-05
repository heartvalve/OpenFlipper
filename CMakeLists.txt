cmake_minimum_required (VERSION 2.6)

project (OpenFlipper)

# add our macro directory to cmake search path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# include our cmake files
include (ACGCommon)

acg_get_version ()
acg_get_version (OpenMesh)

include (package)
include (ACGOutput)

acg_qt4 (REQUIRED 4.5)
acg_openmp ()
acg_ftgl ()

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
acg_set (OPENFLIPPER_PLUGINS "")

# add the basic componenets
add_subdirectory (OpenMesh/src/OpenMesh/Core)
add_subdirectory (OpenMesh/src/OpenMesh/Tools)
add_subdirectory (ACG)
add_subdirectory (OpenFlipper/PluginLib)
add_subdirectory (OpenFlipper/CoreApp)


# search all libs in the libs directory 
file (
  GLOB _local_libs_in
  RELATIVE "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/libs/*/CMakeLists.txt"
)

# add all found libs to the build
foreach (_localLib ${_local_libs_in})
  get_filename_component (_local_lib_dir ${_localLib} PATH)
  add_subdirectory (${CMAKE_SOURCE_DIR}/${_local_lib_dir})
endforeach ()

# add plugins
of_add_plugins ()

if (APPLE)
  # prepare bundle generation cmake file and add a build target for it
  configure_file ("${CMAKE_SOURCE_DIR}/OpenFlipper/cmake/fixbundle.cmake.in"
     "${CMAKE_BINARY_DIR}/fixbundle.cmake" @ONLY IMMEDIATE)
  add_custom_target (fixbundle ALL
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/fixbundle.cmake"
  )
  # let bundle generation depend on all targets
  add_dependencies (fixbundle OpenFlipper PluginLib ${OPENFLIPPER_PLUGINS})

  # install bundle
  install (DIRECTORY ${CMAKE_BINARY_DIR}/Build/OpenFlipper.app DESTINATION .
           USE_SOURCE_PERMISSIONS)
endif ()

# display results
acg_print_configure_header (OPENFLIPPER "OpenFlipper")
of_print_plugin_stats ()
acg_print_configure_footer ()
