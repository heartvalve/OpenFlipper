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
  ../widgets/videoCaptureDialog
  ../widgets/snapshotDialog
)

# collect all header,source and ui files
acg_append_files (headers "*.hh" ${directories})
acg_append_files (sources "*.cc" ${directories})
acg_append_files (ui "*.ui" ${directories})

# remove template cc files from source file list
acg_drop_templates (sources)

# genereate uic and moc targets
acg_qt4_autouic (uic_targets ${ui})
acg_qt4_automoc (moc_targets ${headers})

if (WIN32)
  # add app icon rc file to windows build
  acg_add_executable (OpenFlipper WIN32 ${CMAKE_CURRENT_SOURCE_DIR}/CoreApp.rc ${uic_targets} ${sources} ${headers} ${moc_targets})
  # link to qtmain library to get WinMain function for a non terminal app
  target_link_libraries (OpenFlipper ${QT_QTMAIN_LIBRARY})
elseif (APPLE)
  # generate bundle on mac
  acg_add_executable (OpenFlipper MACOSX_BUNDLE ${uic_targets} ${sources} ${headers} ${moc_targets})
else ()
  acg_add_executable (OpenFlipper ${uic_targets} ${sources} ${headers} ${moc_targets})
endif ()

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

if (APPLE)
   # create bundle in "Build" directory and set icon
   # no install needed here, because the whole bundle will be installed in the
   # toplevel CMakeLists.txt
   set_target_properties (
      OpenFlipper PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build"
      MACOSX_BUNDLE_INFO_STRING "ACG OpenFlipper"
      MACOSX_BUNDLE_ICON_FILE "OpenFlipper.icns"
   )
   add_custom_command(TARGET OpenFlipper POST_BUILD                                                              
     COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/../Icons/OpenFlipper_Icon.icns" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/OpenFlipper.icns"                         
   )    
endif ()


# copy all needed files to "Build" directory to allow a direct execution from it
# the copy_after_build macro (common.cmake) copies the whole directory without svn files
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Shaders" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Shaders")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Textures" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Textures")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Scripts" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Scripts")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Licenses" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Licenses")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Icons" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Icons")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Fonts" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Fonts")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Docs" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Docs")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Help" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help")


if (NOT APPLE)
  # install all needed files
  # the install_dir macro (common.cmake) installs the whole directory without svn files
  # no install for mac needed here, because the whole bundle will be installed in the
  # toplevel CMakeLists.txt
  acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Shaders" "${ACG_PROJECT_DATADIR}/Shaders")
  acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Textures" "${ACG_PROJECT_DATADIR}/Textures")
  acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Scripts" "${ACG_PROJECT_DATADIR}/Scripts")
  acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Icons" "${ACG_PROJECT_DATADIR}/Icons")
  acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Licenses" "${ACG_PROJECT_DATADIR}/Licenses")
  acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Fonts" "${ACG_PROJECT_DATADIR}/Fonts")
  acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Docs" "${ACG_PROJECT_DATADIR}/Docs")
  acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Help" "${ACG_PROJECT_DATADIR}/Help")
endif ()
