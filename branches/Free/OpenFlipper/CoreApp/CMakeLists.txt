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

# Linking for apple is special here as the linker pulls in the dependencies, we have to set them like in PluginLib!
if( APPLE )

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

  link_directories (
    ${GLEW_LIBRARY_DIR}
    ${ADDITIONAL_PLUGINLIB_LINK_DIRS}
  )

else (APPLE)
   link_directories (
      ${GLEW_LIBRARY_DIR}
   )
endif(APPLE)

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
  ../widgets/processManagerWidget
  ../widgets/loadWidget
  ../widgets/optionsWidget
  ../widgets/PluginDialog
  ../widgets/viewModeWidget
  ../widgets/videoCaptureDialog
  ../widgets/snapshotDialog
  ../widgets/stereoSettingsWidget
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

# Mark this build part as building OpenFlippers Core
add_definitions(-DOPENFLIPPERCORE )

# Build app store compliant
if ( OPENFLIPPER_BUILD_APP_STORE_COMPLIANT )
   add_definitions( -DOPENFLIPPER_BUILD_APP_STORE_COMPLIANT )
endif()


if (WIN32)
  add_definitions( -DACGDLL
                   -DUSEACG )
endif ()

# ====================================================================================
# Get list of  ll plugin dependencies and handle special ones
# where we need to link the library directly into the core to prevent 
# linker problems
# ====================================================================================
# Get the property containing the list of all dependencies of the plugins
# List is created when the plugins are added
get_property( global_dependency_list GLOBAL PROPERTY GLOBAL_PLUGIN_DEPENDENCIES_LIST)

set(COREAPP_ADDITIONAL_LINK_LIBRARIES "")

# Special Handling for mpi
list( FIND global_dependency_list "MPI" found)
if ( NOT found EQUAL -1 )
  find_package(MPI)
  list(APPEND COREAPP_ADDITIONAL_LINK_LIBRARIES  ${MPI_LIBRARIES})
endif()

# Special Handling for openni
list( FIND global_dependency_list "OPENNI" found)
if ( NOT found EQUAL -1 )
  find_package(OPENNI)
  list(APPEND COREAPP_ADDITIONAL_LINK_LIBRARIES  ${OPENNI_LIBRARY})
endif()

# ====================================================================================
# Linker Dependencies for Core app
# ====================================================================================


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
  ${COREAPP_ADDITIONAL_LINK_LIBRARIES}
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

 if ( EXISTS ${CMAKE_SOURCE_DIR}/branding )

    IF ( NOT APPLE_BUNDLE_ICON )
        # option to set the used Icon for OpenFlipper
        set ( APPLE_BUNDLE_ICON "${CMAKE_SOURCE_DIR}\\branding\\mac\\OpenFlipper_Icon.icns" CACHE FILEPATH "Path to the Bundle Icon" )
    ENDIF(NOT APPLE_BUNDLE_ICON )    

    IF ( NOT APPLE_INFO_PLIST )
      if ( EXISTS ${CMAKE_SOURCE_DIR}/branding/mac/Info.plist )
        # option to configure the custom Info.plist
        set ( APPLE_INFO_PLIST "${CMAKE_SOURCE_DIR}\\branding\\mac\\Info.plist" CACHE FILEPATH "Path to the Info.plist file for Bundle" )
      else ( EXISTS ${CMAKE_SOURCE_DIR}/branding/mac/Info.plist )
        # option to configure a custum Info.plist but configure with default one
        set ( APPLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/../installer/mac/Info.plist" CACHE FILEPATH "Path to the Info.plist file for Bundle" )
      endif( EXISTS ${CMAKE_SOURCE_DIR}/branding/mac/Info.plist )
    ENDIF(NOT APPLE_INFO_PLIST )

  else()

    IF ( NOT APPLE_BUNDLE_ICON )
        # option to set the used Icon for OpenFlipper
        set ( APPLE_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/../Icons/OpenFlipper_Icon.icns" CACHE FILEPATH "Path to the Bundle Icon" )
    ENDIF(NOT APPLE_BUNDLE_ICON )

    IF ( NOT APPLE_INFO_PLIST )
        # option to configure a custum Info.plist but configure with default one
        set ( APPLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/../installer/mac/Info.plist" CACHE FILEPATH "Path to the Info.plist file for Bundle" )
    ENDIF(NOT APPLE_INFO_PLIST )

  endif()
  
 
   add_custom_command(TARGET OpenFlipper POST_BUILD                                                              
     COMMAND ${CMAKE_COMMAND} -E copy_if_different "${APPLE_BUNDLE_ICON}" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/OpenFlipper.icns"            
   )    

   # create bundle in "Build" directory and set icon
   # no install needed here, because the whole bundle will be installed in the
   # toplevel CMakeLists.txt
   set_target_properties (
      OpenFlipper PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build"
      MACOSX_BUNDLE_INFO_STRING "ACG OpenFlipper"
      MACOSX_BUNDLE_ICON_FILE "OpenFlipper.icns"
      MACOSX_BUNDLE_INFO_PLIST ${APPLE_INFO_PLIST}
   )

endif ()

# Translations
set (trans_files ${sources})
list (APPEND trans_files ${ui})
list (APPEND trans_files ${headers})
acg_add_translations (OpenFlipper "de_DE" "${trans_files}")


# copy all needed files to "Build" directory to allow a direct execution from it
# the copy_after_build macro (common.cmake) copies the whole directory without svn files
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Shaders" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Shaders")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Textures" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Textures")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Scripts" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Scripts")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Licenses" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Licenses")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Icons" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Icons")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Fonts" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Fonts")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Documentation/QtHelpResources" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help")
acg_copy_after_build (OpenFlipper "${CMAKE_CURRENT_SOURCE_DIR}/../Documentation/DocBase" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc")

if ( EXISTS ${CMAKE_SOURCE_DIR}/branding )
  if ( WIN32 ) 
    acg_copy_after_build (OpenFlipper "${CMAKE_SOURCE_DIR}/branding/win" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Icons")
  elseif( APPLE )
    acg_copy_after_build (OpenFlipper "${CMAKE_SOURCE_DIR}/branding/mac" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Icons")
  elseif()
    acg_copy_after_build (OpenFlipper "${CMAKE_SOURCE_DIR}/branding/linux" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Icons")
  endif()
endif()


if ( APPLE ) 
  # we need qt_menu.nib in our resources dir so try to get it

 if ( EXISTS /opt/local/lib/Resources/qt_menu.nib )
  acg_copy_after_build( OpenFlipper "/opt/local/lib/Resources/qt_menu.nib" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/qt_menu.nib")
 elseif ( EXISTS /usr/bin/qt_menu.nib )
   acg_copy_after_build( OpenFlipper "/usr/bin/qt_menu.nib" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/qt_menu.nib")
 elseif ( EXISTS "/opt/local/libexec/qt4-mac/lib/QtGui.framework/Versions/4/Resources/qt_menu.nib" )
   acg_copy_after_build( OpenFlipper "/opt/local/libexec/qt4-mac/lib/QtGui.framework/Versions/4/Resources/qt_menu.nib" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/qt_menu.nib")
 endif ()

 
 
 

endif( APPLE )



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
 # acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/../Help" "${ACG_PROJECT_DATADIR}/Help")

if ( EXISTS ${CMAKE_SOURCE_DIR}/branding )
  if ( WIN32 ) 
    acg_install_dir ( "${CMAKE_SOURCE_DIR}/branding/win" "${ACG_PROJECT_DATADIR}/Icons")
   endif()
endif()


#todo install splash file for windows and Linux branding
endif ()


