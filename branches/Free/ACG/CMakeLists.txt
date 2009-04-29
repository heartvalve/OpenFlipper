include (common)

# find needed packages
find_package (OpenGL)
find_package (GLUT)
find_package (GLEW)
ftgl ()

include_directories (
  ..
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/ShaderUtils
  ${CMAKE_CURRENT_BINARY_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
)

link_directories (
  ${GLEW_LIBRARY_DIR}
)

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
)

# generate dllexport macros on windows
if (WIN32)
  add_definitions(-DACGDLL)
endif ()

# collect all header,source and ui files
append_files (headers "*.hh" ${directories})
append_files (sources "*.cc" ${directories})
append_files (ui "*.ui" ${directories})

# remove template cc files from source file list
drop_templates (sources)

# genereate uic and moc targets
qt4_autouic (uic_targets ${ui})
qt4_automoc (moc_targets ${headers})

add_library (ACG SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})

# set common target properties defined in common.cmake
set_target_props (ACG)

target_link_libraries (ACG 
  OpenMeshCore
  OpenMeshTools
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARY}
  ${GLUT_LIBRARIES}
  ${FTGL_LIBS}
)

# no install on mac, because the whole bundle will be installed in the
# toplevel CMakeLists.txt
if (NOT APPLE)
  install (TARGETS ACG DESTINATION ${OPENFLIPPER_LIBDIR})
endif ()

if (WIN32)
  # copy dll file to "Build" directory
  # Visual studio will create this file in a subdirectory so we can't use
  # LIBRARY_OUTPUT_DIRECTORY directly here
  add_custom_command (TARGET ACG POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E
                      copy_if_different 
                        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ACG.dll
                        ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_LIBDIR}/ACG.dll
                      )
endif ()


