include (common)

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

if (WIN32)
  add_definitions(-DACGDLL)
endif ()

append_files (headers "*.hh" ${directories})
append_files (sources "*.cc" ${directories})
append_files (ui "*.ui" ${directories})

drop_templates (sources)

qt4_autouic (uic_targets ${ui})
qt4_automoc (moc_targets ${headers})

add_library (ACG SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})

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

if (NOT APPLE)
  install (TARGETS ACG DESTINATION ${OPENFLIPPER_LIBDIR})
endif ()

if (WIN32)
  add_custom_command (TARGET ACG POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E
                      copy_if_different 
                        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ACG.dll
                        ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_LIBDIR}/ACG.dll
                      )
endif ()


