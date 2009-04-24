include (common)

find_package (OpenGL)
find_package (GLUT)
find_package (GLEW)
ftgl ()

include_directories (
  ..
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
)

link_directories (
  ${GLEW_LIBRARY_DIR}
)

set (directories 
  ../BasePlugin 
  ../ACGHelper 
  ../common 
  ../common/bsp  
  ../INIFile 
  ../widgets/glWidget
)

if (WIN32)
  add_definitions (
    -DPLUGINLIBDLL
  )
endif ()

append_files (headers "*.hh" ${directories})
append_files (sources "*.cc" ${directories})
append_files (ui "*.ui" ${directories})

append_files_recursive (headers "*.hh" ${CMAKE_SOURCE_DIR}/ObjectTypes)
append_files_recursive (sources "*.cc" ${CMAKE_SOURCE_DIR}/ObjectTypes)

drop_templates (sources)

qt4_autouic (uic_targets ${ui})
qt4_automoc (moc_targets ${headers})

add_library (PluginLib SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})

set_target_props (PluginLib)

target_link_libraries (PluginLib
  OpenMeshCore
  OpenMeshTools
  ACG
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARY}
  ${GLUT_LIBRARIES}
  ${FTGL_LIBS}
)

if (NOT APPLE)
  install (TARGETS PluginLib DESTINATION ${OPENFLIPPER_LIBDIR})
endif ()

if (WIN32)
  add_custom_command (TARGET PluginLib POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E
                      copy_if_different 
                        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/PluginLib.dll
                        ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_LIBDIR}/PluginLib.dll)
endif ()