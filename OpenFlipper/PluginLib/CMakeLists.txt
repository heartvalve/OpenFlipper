include (common)

# find needed packages
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

# source code directories
set (directories 
  ../BasePlugin 
  ../ACGHelper 
  ../common 
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

# collect all header,source and ui files
append_files (headers "*.hh" ${directories})
append_files (sources "*.cc" ${directories})
append_files (ui "*.ui" ${directories})

# add additional object type sources
append_files_recursive (headers "*.hh" ${CMAKE_SOURCE_DIR}/ObjectTypes)
append_files_recursive (sources "*.cc" ${CMAKE_SOURCE_DIR}/ObjectTypes)

# remove template cc files from source file list
drop_templates (sources)

# genereate uic and moc targets
qt4_autouic (uic_targets ${ui})
qt4_automoc (moc_targets ${headers})

add_library (PluginLib SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})

# set common target properties defined in common.cmake
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
  # no install on mac, because the whole bundle will be installed in the
  # toplevel CMakeLists.txt
  install (TARGETS PluginLib DESTINATION ${OPENFLIPPER_LIBDIR})
endif ()

if (WIN32)
  # copy dll file to "Build" directory
  # Visual studio will create this file in a subdirectory so we can't use
  # LIBRARY_OUTPUT_DIRECTORY directly here
  add_custom_command (TARGET PluginLib POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E
                      copy_if_different 
                        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/PluginLib.dll
                        ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_LIBDIR}/PluginLib.dll)
endif ()