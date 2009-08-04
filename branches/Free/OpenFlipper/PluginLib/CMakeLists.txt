include (ACGCommon)

include_directories (
  ..
  ${CMAKE_SOURCE_DIR}/OpenMesh/src
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

set (PLUGINLIB_FILES_TO_TRANSLATE ${PLUGINLIB_FILES_TO_TRANSLATE} ${sources} ${ui} ${headers})
# generate/use translation files
# run with UPDATE_TRANSLATIONS set to on to build qm files
option (UPDATE_TRANSLATIONS "Update source translation translations/PluginLib_de_DE.ts files (WARNING: make clean will delete the source .ts files! Danger!)")
if (UPDATE_TRANSLATIONS)
  qt4_create_translation(QM_FILES ${PLUGINLIB_FILES_TO_TRANSLATE} translations/PluginLib_de_DE.ts )
else (UPDATE_TRANSLATIONS)
  qt4_add_translation(QM_FILES translations/PluginLib_de_DE.ts )
endif (UPDATE_TRANSLATIONS)

# create a target for the translation files ( and object files )
# Use this target, to update only the translations
add_custom_target (translations_target_pluginlib DEPENDS ${QM_FILES})

# Build translations with the application
add_dependencies( PluginLib translations_target_pluginlib )

# Copy all translation files after build
of_copy_translations( PluginLib )

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
