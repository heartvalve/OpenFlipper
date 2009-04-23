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
  .. 
  ../Core
  ../Logging
  ../Scripting
  ../Scripting/scriptPrototypes
  ../Scripting/scriptWrappers
  ../SimpleOpt
  ../widgets/aboutWidget 
  ../widgets/addEmptyWidget
  ../widgets/loggerWidget
  ../widgets/coreWidget 
  ../widgets/helpWidget
  ../widgets/loadWidget
  ../widgets/optionsWidget
  ../widgets/PluginDialog
  ../widgets/viewModeWidget
)

append_files (headers "*.hh" ${directories})
append_files (sources "*.cc" ${directories})
append_files (ui "*.ui" ${directories})

drop_templates (sources)

qt4_autouic (uic_targets ${ui})
qt4_automoc (moc_targets ${headers})

if (WIN32)
  add_executable (OpenFlipper ${CMAKE_CURRENT_SOURCE_DIR}/CoreApp.rc ${uic_targets} ${sources} ${headers} ${moc_targets})
else ()
  add_executable (OpenFlipper ${uic_targets} ${sources} ${headers} ${moc_targets})
endif ()

set_target_props (OpenFlipper)

target_link_libraries (OpenFlipper
  OpenMeshCore
  OpenMeshTools
  ACG
  PluginLib
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARY}
  ${GLUT_LIBRARIES}
  ${FTGL_LIBS}
)

install (TARGETS OpenFlipper DESTINATION ${OPENFLIPPER_BINDIR})

if (WIN32)
  add_custom_command (TARGET OpenFlipper POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E
                      copy_if_different 
                        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/OpenFlipper.exe
                        ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_BINDIR}/OpenFlipper.exe)
endif ()

add_custom_command(TARGET OpenFlipper POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../Shaders ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../Textures ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Textures
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../Icons ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Icons
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../Fonts ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Fonts
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../Docs ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Docs
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../Help ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Help
)

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/../Shaders
  ${CMAKE_CURRENT_SOURCE_DIR}/../Textures
  ${CMAKE_CURRENT_SOURCE_DIR}/../Icons
  ${CMAKE_CURRENT_SOURCE_DIR}/../Fonts
  ${CMAKE_CURRENT_SOURCE_DIR}/../Docs
  ${CMAKE_CURRENT_SOURCE_DIR}/../Help
  DESTINATION ${OPENFLIPPER_DATADIR}
)
