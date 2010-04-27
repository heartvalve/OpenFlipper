# option to disable inclusion of qmake build system into source package
option (
  DISABLE_QMAKE_BUILD
  "Disable inclusion of qmake build system into source package"
  OFF
)

# set name
set (CPACK_PACKAGE_NAME "OpenFlipper")
set (CPACK_PACKAGE_VENDOR "ACG")

# set version
set (CPACK_PACKAGE_VERSION_MAJOR "${OPENFLIPPER_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${OPENFLIPPER_VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "${OPENFLIPPER_VERSION_PATCH}")
set (CPACK_PACKAGE_VERSION "${OPENFLIPPER_VERSION}")

# addition package info
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenFlipper Mesh manipulation too")
set (CPACK_PACKAGE_EXECUTABLES "OpenFlipper;OpenFlipper")
set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_SOURCE_STRIP_FILES "")

# set (CPACK_PACKAGE_DESCRIPTION_FILE "/home/andy/vtk/CMake/Copyright.txt")
# set (CPACK_RESOURCE_FILE_LICENSE "/home/andy/vtk/CMake/Copyright.txt")
# set (CPACK_RESOURCE_FILE_README "/home/andy/vtk/CMake/Templates/CPack.GenericDescription.txt")
# set (CPACK_RESOURCE_FILE_WELCOME "/home/andy/vtk/CMake/Templates/CPack.GenericWelcome.txt")

set (CPACK_STRIP_FILES "bin/OpenFlipper")

# source package generation
set (CPACK_SOURCE_GENERATOR "TGZ;TBZ2;ZIP")

# ignored files in source package
set (CPACK_SOURCE_IGNORE_FILES  "\\\\.#;/#;.*~")
list (APPEND CPACK_SOURCE_IGNORE_FILES "/\\\\.git")
list (APPEND CPACK_SOURCE_IGNORE_FILES "/\\\\.svn")
list (APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}")
list (APPEND CPACK_SOURCE_IGNORE_FILES "Makefile")
list (APPEND CPACK_SOURCE_IGNORE_FILES "Makefile\\\\..*")
list (APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.moc\\\\.cpp$")
list (APPEND CPACK_SOURCE_IGNORE_FILES "CMakeCache.txt")
list (APPEND CPACK_SOURCE_IGNORE_FILES "CMakeFiles")

list (APPEND CPACK_SOURCE_IGNORE_FILES "/.*_(32|64)_Debug/")
list (APPEND CPACK_SOURCE_IGNORE_FILES "/.*_(32|64)_Release/")

list (APPEND CPACK_SOURCE_IGNORE_FILES "/MacOS")
list (APPEND CPACK_SOURCE_IGNORE_FILES "/WIN")
list (APPEND CPACK_SOURCE_IGNORE_FILES "/tmp/")

list (APPEND CPACK_SOURCE_IGNORE_FILES "/.*\\\\.kdevelop")
list (APPEND CPACK_SOURCE_IGNORE_FILES "/.*\\\\.kdevses")

list (APPEND CPACK_SOURCE_IGNORE_FILES "/ACG/lib/")
list (APPEND CPACK_SOURCE_IGNORE_FILES "/ACG/include/")

if (DISABLE_QMAKE_BUILD)
  list (APPEND CPACK_SOURCE_IGNORE_FILES "/.*\\\\.pro")
  list (APPEND CPACK_SOURCE_IGNORE_FILES "/qmake/")
  list (APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.qmake\\\\.cache")
endif ()

# filter out all disabled plugins
file (
  GLOB _plugins_in
  RELATIVE "${CMAKE_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/Plugin-*"
)
foreach (_plugin ${_plugins_in})
  string (REPLACE "Plugin-" "" _plugin_name ${_plugin})
  string (TOUPPER ${_plugin_name} _PLUGIN)
  if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${_plugin}/CMakeLists.txt AND DISABLE_QMAKE_BUILD)
    list (APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/${_plugin}")
  elseif (DISABLE_PLUGIN_${_PLUGIN})
    list (APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/${_plugin}")
  endif ()
endforeach ()


if (WIN32)
  if ( EXISTS ${CMAKE_SOURCE_DIR}/branding ) 
    IF ( NOT WINDOWS_INSTALLER_ICON )
        # option to set the used Icon for OpenFlipper
        set ( WINDOWS_INSTALLER_ICON "${CMAKE_SOURCE_DIR}\\branding\\win\\OpenFlipper_Icon_128x128x32.ico" CACHE FILEPATH "Path to the Executables Icon" )
    ENDIF(NOT WINDOWS_INSTALLER_ICON )

    IF ( NOT WINDOWS_INSTALLER_IMAGE_ICON )
        # option to set the used Icon for OpenFlipper
        set ( WINDOWS_INSTALLER_IMAGE_ICON "${CMAKE_SOURCE_DIR}/branding/win/installer.bmp" CACHE FILEPATH "Path to the Installer Image Icon ( This has to be a bmp )" )
    ENDIF(NOT WINDOWS_INSTALLER_IMAGE_ICON )

  else()

    IF ( NOT WINDOWS_INSTALLER_ICON )
        # option to set the used Icon for OpenFlipper
        set ( WINDOWS_INSTALLER_ICON "${CMAKE_SOURCE_DIR}\\OpenFlipper\\Icons\\OpenFlipper_Icon_128x128x32.ico" CACHE FILEPATH "Path to the Executables Icon" )
    ENDIF(NOT WINDOWS_INSTALLER_ICON )

    IF ( NOT WINDOWS_INSTALLER_IMAGE_ICON )
        # option to set the used Icon for OpenFlipper
        set ( WINDOWS_INSTALLER_IMAGE_ICON "${CMAKE_SOURCE_DIR}/OpenFlipper/installer/win/installer.bmp" CACHE FILEPATH "Path to the Installer Image Icon ( This has to be a bmp )" )
    ENDIF(NOT WINDOWS_INSTALLER_IMAGE_ICON )

  endif()

  string(REGEX REPLACE "/" "\\\\\\\\" CLEAN_WINDOWS_INSTALLER_IMAGE_ICON "${WINDOWS_INSTALLER_IMAGE_ICON}"  )

  # window NSIS installer
  set (CPACK_GENERATOR "NSIS")
  set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
  set (CPACK_NSIS_DISPLAY_NAME "OpenFlipper v${CPACK_PACKAGE_VERSION}")

  set (CPACK_NSIS_MUI_ICON ${WINDOWS_INSTALLER_ICON} )
  # we need a real uninstaller icon here and we have to define both to make the installer icon work
  set (CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}\\\\OpenFlipper\\\\Icons\\\\OpenFlipper_Icon_128x128x32.ico")

  set (CPACK_PACKAGE_ICON "${CLEAN_WINDOWS_INSTALLER_IMAGE_ICON}")
  set (CPACK_NSIS_HELP_LINK "http:\\\\www.openflipper.org")
  set (CPACK_NSIS_URL_INFO_ABOUT "http:\\\\www.openflipper.org")

  # Set the new Icon Name for OpenFlipper Core Application
  
  set (CPACK_PACKAGE_EXECUTABLES "OpenFlipper;OpenFlipper")
  # TODO: fillme
  # set (CPACK_NSIS_CONTACT "")

  set (CPACK_NSIS_CREATE_ICONS
       "CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\OpenFlipper.lnk\\\" \\\"$INSTDIR\\\\OpenFlipper.exe\\\""
      )
  set (CPACK_NSIS_DELETE_ICONS
       "Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\OpenFlipper.lnk\\\""
      )

  # append dll's to installed package
  if (EXISTS ${CMAKE_SOURCE_DIR}/WIN)
    file (GLOB _files "${CMAKE_SOURCE_DIR}/WIN/DLLs/DLLs 32 debug/*.dll")
    install(FILES ${_files}
      DESTINATION ${ACG_PROJECT_BINDIR}
      CONFIGURATIONS Debug
    )
    file (GLOB _files "${CMAKE_SOURCE_DIR}/WIN/DLLs/DLLs 32 release/*.dll")
    install (FILES ${_files}
      DESTINATION ${ACG_PROJECT_BINDIR}
      CONFIGURATIONS Release
    )
    install (FILES "${CMAKE_SOURCE_DIR}/WIN/DLLs/Redistributables/Visual Studio 2008/vcredist_x86.exe"
      DESTINATION ${ACG_PROJECT_BINDIR}
    )
    set (CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\vcredist_x86.exe\\\" /q:a'")
  endif ()
elseif (APPLE)

  if ( EXISTS ${CMAKE_SOURCE_DIR}/branding )
    IF ( NOT APPLE_INSTALLER_ICON )
        # option to set the used Icon for OpenFlipper
        set ( APPLE_INSTALLER_ICON "${CMAKE_SOURCE_DIR}\\branding\\mac\\installer-icon.ico" CACHE FILEPATH "Path to the Executables Icon" )
    ENDIF(NOT APPLE_INSTALLER_ICON )

    IF ( NOT APPLE_INSTALLER_IMAGE_ICON )
        # option to set the used Icon for OpenFlipper
        set ( APPLE_INSTALLER_IMAGE_ICON "${CMAKE_SOURCE_DIR}/branding/mac/installer.bmp" CACHE FILEPATH "Path to the Installer Image Icon ( This has to be a bmp )" )
    ENDIF(NOT APPLE_INSTALLER_IMAGE_ICON )

  else()

    IF ( NOT APPLE_INSTALLER_ICON )
        # option to set the used Icon for OpenFlipper
        set ( APPLE_INSTALLER_ICON "${CMAKE_SOURCE_DIR}\\OpenFlipper\\Icons\\OpenFlipper_Icon_128x128x32.ico" CACHE FILEPATH "Path to the Executables Icon" )
    ENDIF(NOT APPLE_INSTALLER_ICON )

    IF ( NOT APPLE_INSTALLER_IMAGE_ICON )
        # option to set the used Icon for OpenFlipper
        set ( APPLE_INSTALLER_IMAGE_ICON "${CMAKE_SOURCE_DIR}/OpenFlipper/installer/mac/installer.bmp" CACHE FILEPATH "Path to the Installer Image Icon ( This has to be a bmp )" )
    ENDIF(NOT APPLE_INSTALLER_IMAGE_ICON )

  endif()

  set (CPACK_NSIS_MUI_ICON ${APPLE_INSTALLER_ICON} )

  # we need a real uninstaller icon here and we have to define both to make the installer icon work
  set (CPACK_NSIS_MUI_UNIICON "${APPLE_INSTALLER_ICON}")

  set (CPACK_PACKAGE_ICON "${APPLE_INSTALLER_IMAGE_ICON}")


  # apple Drag'n'Drop installer package
  set (CPACK_GENERATOR "DragNDrop")
#  set (CPACK_GENERATOR "DragNDrop;TGZ")
  

endif ()

# has to be last
if ( NOT WIN32)
#  set (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP "true")
#  include (InstallRequiredSystemLibraries)
#  install (PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ${ACG_PROJECT_BINDIR})
#else ()
  include (InstallRequiredSystemLibraries)
endif ()


include (CPack)

if (NOT WIN32 AND NOT APPLE)
  # no binary target for linux
  file (REMOVE "${CMAKE_BINARY_DIR}/CPackConfig.cmake")
endif ()

# cmake doesn't create a source package target, so we have to add our own
if (EXISTS "${CMAKE_BINARY_DIR}/CPackSourceConfig.cmake")
  add_custom_target (PACKAGE_SOURCE
    ${CMAKE_CPACK_COMMAND} --config "${CMAKE_BINARY_DIR}/CPackSourceConfig.cmake"
  )
endif ()
