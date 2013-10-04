include (ACGCommon)

set ( ADDITIONAL_PLUGINLIB_LIBS "")
set ( ADDITIONAL_PLUGINLIB_INCS "")
set ( ADDITIONAL_PLUGINLIB_COMPILER_FLAGS "")
set ( ADDITIONAL_PLUGINLIB_LINK_DIRS "")

# search all ObjectTypes in the ObjectType directory for additional build information
file (
  GLOB _plugin_buildinfos
  RELATIVE "${CMAKE_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/ObjectTypes/*/CMakeLists.txt"
)

# include all cmake files found for objecttypes here
foreach ( _buildInfo ${_plugin_buildinfos})
  include ("${CMAKE_SOURCE_DIR}/${_buildInfo}")
endforeach ()

include_directories (
  ..
  ${OPENMESH_INCLUDE_DIR}
  ${OPENVOLUMEMESH_INCLUDE_DIR}
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
  ${ADDITIONAL_PLUGINLIB_LINK_DIRS}
)

#=======================================================
# General defines
#=======================================================

# generate dllexport macros on windows
if (WIN32)
  add_definitions (
    -DPLUGINLIBDLL
    -DACGDLL
    -DUSEACG
  )
endif ()

# define core version
add_definitions (
    -DCORE_VERSION="${OPENFLIPPER_VERSION}"
)

# Mark this build part as building OpenFLippers Core
add_definitions(-DOPENFLIPPERPLUGINLIB )

#=======================================================
# Add possibility to debug OpenFlipperSettings
#=======================================================

if ( NOT DEFINED OPENFLIPPER_SETTINGS_DEBUG )
  set( OPENFLIPPER_SETTINGS_DEBUG false CACHE BOOL "Enable or disable debugging of OpenFlipperSettings" )
  mark_as_advanced(OPENFLIPPER_SETTINGS_DEBUG)
endif()

if ( OPENFLIPPER_SETTINGS_DEBUG )
 add_definitions(-DOPENFLIPPER_SETTINGS_DEBUG )
endif()

#=======================================================
# Collect files to compile
#=======================================================

# source code directories
set (directories
  ../BasePlugin
  ../ACGHelper
  ../common
  ../threads
  ../common/bsp
  ../INIFile
  ../widgets/glWidget
  ../publicWidgets/objectSelectionWidget
  ../widgets/filePicker 
)

# collect all header,source and ui files
acg_append_files (headers "*.hh" ${directories})
acg_append_files (sources "*.cc" ${directories})
acg_append_files (ui "*.ui" ${directories})

# add additional object type sources
acg_append_files_recursive (headers "*.hh" ${CMAKE_SOURCE_DIR}/ObjectTypes)
acg_append_files_recursive (sources "*.cc" ${CMAKE_SOURCE_DIR}/ObjectTypes)
acg_append_files_recursive (ui "*.ui" ${CMAKE_SOURCE_DIR}/ObjectTypes)

# remove template cc files from source file list
acg_drop_templates (sources)

# genereate uic and moc targets
if (QT5_FOUND)
  acg_qt5_autouic (uic_targets ${ui})
  acg_qt5_automoc (moc_targets ${headers})
elseif (QT4_FOUND)
  acg_qt4_autouic (uic_targets ${ui})
  acg_qt4_automoc (moc_targets ${headers})
endif ()

#=======================================================
# Create library
#=======================================================

if (QT5_FOUND)
  acg_add_library (OpenFlipperPluginLib SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})
elseif (QT4_FOUND)
  acg_add_library (OpenFlipperPluginLib SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})
endif ()

if( ADDITIONAL_PLUGINLIB_COMPILER_FLAGS )
  set_target_properties (
    OpenFlipperPluginLib PROPERTIES
    COMPILE_FLAGS ${ADDITIONAL_PLUGINLIB_COMPILER_FLAGS}
  )
endif()

target_link_libraries (OpenFlipperPluginLib
  OpenMeshCore
  OpenMeshTools
  ${OPENVOLUMEMESH_LIBRARY}
  ACG
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARY}
  ${GLUT_LIBRARIES}
  ${FTGL_LIBS}
  ${ADDITIONAL_PLUGINLIB_LIBS}
)

#=======================================================
# Setup translation files for OpenFlipperPluginLib
#=======================================================

set (trans_files ${sources})
list (APPEND trans_files ${ui})
list (APPEND trans_files ${headers})
acg_add_translations (OpenFlipperPluginLib "de_DE" "${trans_files}")




