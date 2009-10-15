include (ACGCommon)


set ( ADDITIONAL_PLUGINLIB_LIBS "")
set ( ADDITIONAL_PLUGINLIB_INCS "")
set ( ADDITIONAL_PLUGINLIB_COMPILER_FLAGS "")

# search all ObjectTypes in the ObjectType directory for additional build information
file (
  GLOB _plugin_buildinfos
  RELATIVE "${CMAKE_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/ObjectTypes/*/CMakeLists.txt"
)

# include all cmake files fouund for objecttypes here
foreach ( _buildInfo ${_plugin_buildinfos})
  include ("${CMAKE_SOURCE_DIR}/${_buildInfo}")
endforeach ()

include_directories (
  ..
  ${CMAKE_SOURCE_DIR}/OpenMesh/src
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
  ${ADDITIONAL_PLUGINLIB_INCS}
)

link_directories (
  ${GLEW_LIBRARY_DIR}
)

# source code directories
set (directories 
  ../BasePlugin 
  ../ACGHelper 
  ../common 
  ../threads
  ../common/bsp  
  ../INIFile 
  ../widgets/glWidget
)

# generate dllexport macros on windows
if (WIN32)
  add_definitions (
    -DPLUGINLIBDLL
  )
endif ()

# define core version
add_definitions (
    -DCORE_VERSION="${OPENFLIPPER_VERSION}"
)

# collect all header,source and ui files
acg_append_files (headers "*.hh" ${directories})
acg_append_files (sources "*.cc" ${directories})
acg_append_files (ui "*.ui" ${directories})

# add additional object type sources
acg_append_files_recursive (headers "*.hh" ${CMAKE_SOURCE_DIR}/ObjectTypes)
acg_append_files_recursive (sources "*.cc" ${CMAKE_SOURCE_DIR}/ObjectTypes)

# remove template cc files from source file list
acg_drop_templates (sources)

# genereate uic and moc targets
acg_qt4_autouic (uic_targets ${ui})
acg_qt4_automoc (moc_targets ${headers})

acg_add_library (PluginLib SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})
set_target_properties (
  PluginLib PROPERTIES 
  COMPILE_FLAGS ${ADDITIONAL_PLUGINLIB_COMPILER_FLAGS}
)

set (trans_files ${sources})
list (APPEND trans_files ${ui})
list (APPEND trans_files ${headers})
acg_add_translations (PluginLib "de_DE" "${trans_files}")

target_link_libraries (PluginLib
  OpenMeshCore
  OpenMeshTools
  ACG
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARY}
  ${GLUT_LIBRARIES}
  ${FTGL_LIBS}
  ${ADDITIONAL_PLUGINLIB_LIBS}
)
