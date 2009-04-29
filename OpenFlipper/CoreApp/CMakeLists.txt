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

# collect all header,source and ui files
append_files (headers "*.hh" ${directories})
append_files (sources "*.cc" ${directories})
append_files (ui "*.ui" ${directories})

# remove template cc files from source file list
drop_templates (sources)

# genereate uic and moc targets
qt4_autouic (uic_targets ${ui})
qt4_automoc (moc_targets ${headers})

if (WIN32)
  # add app icon rc file to windows build
  add_executable (OpenFlipper WIN32 ${CMAKE_CURRENT_SOURCE_DIR}/CoreApp.rc ${uic_targets} ${sources} ${headers} ${moc_targets})
  # link to qtmain library to get WinMain function for a non terminal app
  target_link_libraries (OpenFlipper ${QT_QTMAIN_LIBRARY})
elseif (APPLE)
  # generate bundle on mac
  add_executable (OpenFlipper MACOSX_BUNDLE ${uic_targets} ${sources} ${headers} ${moc_targets})
else ()
  add_executable (OpenFlipper ${uic_targets} ${sources} ${headers} ${moc_targets})
endif ()

# set common target properties defined in common.cmake
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

if (APPLE)
   # create bundle in "Build" directory and set icon
   # no install needed here, because the whole bundle will be installed in the
   # toplevel CMakeLists.txt
   set_target_properties (
      OpenFlipper PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build"
      MACOSX_BUNDLE_INFO_STRING "ACG OpenFlipper"
      MACOSX_BUNDLE_ICON_FILE "Icons/OpenFlipper_Icon.icns"
   )
else ()
  install (TARGETS OpenFlipper DESTINATION ${OPENFLIPPER_BINDIR})
endif ()

if (WIN32)
  # copy exe file to "Build" directory
  # Visual studio will create this file in a subdirectory so we can't use
  # RUNTIME_OUTPUT_DIRECTORY directly here
  add_custom_command (TARGET OpenFlipper POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E
                      copy_if_different 
                        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/OpenFlipper.exe
                        ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_BINDIR}/OpenFlipper.exe)
endif ()

# copy all needed files to "Build" directory to allow a direct execution from it
# the copy_after_build macro (common.cmake) copies the whole directory without svn files
copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Shaders" "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Shaders")
copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Textures" "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Textures")
copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Icons" "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Icons")
copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Fonts" "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Fonts")
copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Docs" "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Docs")
copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Help" "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_DATADIR}/Help")


if (NOT APPLE)
  # install all needed files
  # the install_dir macro (common.cmake) installs the whole directory without svn files
  # no install for mac needed here, because the whole bundle will be installed in the
  # toplevel CMakeLists.txt
  install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Shaders" "${OPENFLIPPER_DATADIR}/Shaders")
  install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Textures" "${OPENFLIPPER_DATADIR}/Textures")
  install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Icons" "${OPENFLIPPER_DATADIR}/Icons")
  install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Fonts" "${OPENFLIPPER_DATADIR}/Fonts")
  install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Docs" "${OPENFLIPPER_DATADIR}/Docs")
  install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Help" "${OPENFLIPPER_DATADIR}/Help")
endif ()