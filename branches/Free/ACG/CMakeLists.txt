include (ACGCommon)

acg_get_version ()

# Disable Library installation when not building ACG on its own but as part of another project!
if ( ${PROJECT_NAME} MATCHES "OpenFlipper")
  set(ACG_NO_LIBRARY_INSTALL true)
endif()



if (NOT FORCE_QT4)
  # try to use QT5 if possible otherwise stick to QT4
  acg_qt5 ()  
else()
  set (QT_MAX_VERSION 4.9.9)
  set (QT5_FOUND false)
endif()


if (NOT QT5_FOUND)
  message(STATUS "Using QT4 for ACG")
  acg_qt4 (REQUIRED 4.5)
else ()
  message(STATUS "Using QT5 for ACG")
endif ()

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

find_package (OpenMesh)
if (NOT OPENMESH_FOUND)
    message (FATAL_ERROR "OpenMesh not found!")
endif ()

set(INCLUDE_DIRS ${OPENMESH_INCLUDE_DIRS}
                 ${CMAKE_CURRENT_SOURCE_DIR}
                 ${CMAKE_CURRENT_SOURCE_DIR}/ShaderUtils
                 ${CMAKE_CURRENT_BINARY_DIR}
                 ${OPENGL_INCLUDE_DIR}
                 ${GLEW_INCLUDE_DIR}
                 ${GLUT_INCLUDE_DIR} )

set(ADDITIONAL_LINK_LIBRARIES "" )
set(ADDITIONAL_BUILD_DIRS "" )

find_package (QJson)
if (QJSON_FOUND)
    LIST(APPEND INCLUDE_DIRS ${QJSON_INCLUDE_DIR})
    LIST(APPEND ADDITIONAL_LINK_LIBRARIES ${QJSON_LIBRARY})
endif ()

# use Qwt6 for QT5
if (NOT FORCE_QT4 AND QT5_FOUND)
  find_package(Qwt6)

  if (QWT6_FOUND)
    set(INCLUDE_DIRS ${INCLUDE_DIRS} ${QWT6_INCLUDE_DIRS} )
    set(ADDITIONAL_LINK_LIBRARIES ${ADDITIONAL_LINK_LIBRARIES} ${QWT6_LIBRARIES})
    set(ADDITIONAL_BUILD_DIRS "QwtWidgets" )
  endif()
else()
  find_package(Qwt5)

  if(Qwt5_Qt4_FOUND)
    set(INCLUDE_DIRS ${INCLUDE_DIRS} ${Qwt5_INCLUDE_DIR} )
    set(ADDITIONAL_LINK_LIBRARIES ${ADDITIONAL_LINK_LIBRARIES} ${Qwt5_Qt4_LIBRARY})
    set(ADDITIONAL_BUILD_DIRS "QwtWidgets" )
  endif()
endif()

include_directories (
  ..
  ${INCLUDE_DIRS}
)

link_directories (
  ${GLEW_LIBRARY_DIR}
  ${GLUT_LIBRARY_DIR}
)

#===================================================================
# ACG Library setup
#===================================================================

# source code directories
set (directories
  .
  Geometry
  Config
  Geometry/Types
  GL
  Glut
  IO
  Math
  QtWidgets
  Scenegraph
  QtScenegraph
  ShaderUtils
  Utils
  ${ADDITIONAL_BUILD_DIRS}
)

# generate dllexport macros on windows
if (WIN32)
  add_definitions(-DACGDLL)
endif ()

# collect all header,source and ui files
acg_append_files (headers "*.hh" ${directories})
acg_append_files (sources "*.cc" ${directories})
acg_append_files (ui "*.ui" ${directories})

# remove template cc files from source file list
acg_drop_templates (sources)

# genereate uic and moc targets
if (QT5_FOUND)
  acg_qt5_autouic (uic_targets ${ui})
  acg_qt5_automoc (moc_targets ${headers})
elseif (QT4_FOUND)
  acg_qt4_autouic (uic_targets ${ui})
  acg_qt4_automoc (moc_targets ${headers})
endif()


if (QT5_FOUND)
  acg_add_library (ACG SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})
elseif (QT4_FOUND)
  acg_add_library (ACG SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})
endif()
if ( NOT WIN32 )
  set_target_properties (ACG PROPERTIES VERSION ${ACG_VERSION_MAJOR}.${ACG_VERSION_MINOR}
                                      SOVERSION ${ACG_VERSION_MAJOR}.${ACG_VERSION_MINOR})
endif()

if (NOT ACG_PROJECT_MACOS_BUNDLE OR NOT APPLE)
  install (TARGETS ACG DESTINATION ${ACG_PROJECT_LIBDIR})
endif ()

# display results
acg_print_configure_header (ACG "ACG")

target_link_libraries ( ACG  ${OPENMESH_LIBRARIES}
                             ${QT_LIBRARIES}
                             ${OPENGL_LIBRARIES}
                             ${GLEW_LIBRARY}
                             ${GLUT_LIBRARIES}
                             ${ADDITIONAL_LINK_LIBRARIES} )
message("ADDITIONAL_LINK_LIBRARIES ${ADDITIONAL_LINK_LIBRARIES}")

#===================================================================
# Setting up the unit tests
#===================================================================
find_package(GoogleTest)
if (GTEST_FOUND)
  enable_testing()
  file(GLOB_RECURSE TEST_SOURCES tests/*.cc)
  set(TESTED_SOURCES
        Algorithm/DBSCANT.cc
     )
  include_directories(${GTEST_INCLUDE_DIRS} ${OPENMESH_INCLUDE_DIR})
  link_directories ( ${GTEST_LIBRARY_DIR})

  if ( CMAKE_GENERATOR MATCHES "^Visual Studio 11.*" )
     add_definitions( /D _VARIADIC_MAX=10 )
  endif()
 
  add_executable (ACG_tests ${TEST_SOURCES})

  set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
  # run tests in build directory of OF
  if ( ${PROJECT_NAME} MATCHES "OpenFlipper" AND WIN32)
    set(OUT_DIR  ${CMAKE_BINARY_DIR}/Build)
  endif()

  set_target_properties(ACG_tests PROPERTIES
    COMPILE_FLAGS "-UNDEBUG"
    RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUT_DIR}
  )
  
  target_link_libraries(ACG_tests
        ${GTEST_LIBRARIES} ${OPENMESH_LIBRARIES} ACG
  )
  
  add_test(NAME AllTestsIn_ACG_tests COMMAND ${OUT_DIR}/ACG_tests)
endif(GTEST_FOUND)
