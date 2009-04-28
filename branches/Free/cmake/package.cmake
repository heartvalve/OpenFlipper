set (CPACK_PACKAGE_NAME "OpenFlipper")
set (CPACK_PACKAGE_VENDOR "ACG")

set (CPACK_PACKAGE_VERSION_MAJOR "1")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")



set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "CMake is a build tool")
set (CPACK_PACKAGE_EXECUTABLES "OpenFlipper;OpenFlipper")
set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_SOURCE_STRIP_FILES "")

# set (CPACK_PACKAGE_DESCRIPTION_FILE "/home/andy/vtk/CMake/Copyright.txt")
# set (CPACK_RESOURCE_FILE_LICENSE "/home/andy/vtk/CMake/Copyright.txt")
# set (CPACK_RESOURCE_FILE_README "/home/andy/vtk/CMake/Templates/CPack.GenericDescription.txt")
# set (CPACK_RESOURCE_FILE_WELCOME "/home/andy/vtk/CMake/Templates/CPack.GenericWelcome.txt")

set (CPACK_STRIP_FILES "bin/OpenFlipper")

set (CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
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

if (WIN32)
  set (CPACK_GENERATOR "NSIS")
  set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
  set (CPACK_NSIS_DISPLAY_NAME "OpenFlipper v${CPACK_PACKAGE_VERSION}")
  set (CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}\\\\OpenFlipper\\\\Icons\\\\OpenFlipper_Icon_128x128x32.ico")
  # we need a real uninstaller icon here and we have to define both to make the installer icon work
  set (CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}\\\\OpenFlipper\\\\Icons\\\\OpenFlipper_Icon_128x128x32.ico")
  set (CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\OpenFlipper\\\\Icons\\\\installer.bmp")
  set (CPACK_NSIS_HELP_LINK "http:\\\\www.openflipper.org")
  set (CPACK_NSIS_URL_INFO_ABOUT "http:\\\\www.openflipper.org")
  # TODO: fillme
  # set (CPACK_NSIS_CONTACT "")

  if (EXISTS ${CMAKE_SOURCE_DIR}/WIN)
    file (GLOB _files "${CMAKE_SOURCE_DIR}/WIN/DLLs/DLLs 32 debug/*.dll")
    install(FILES ${_files}
      DESTINATION ${OPENFLIPPER_LIBDIR}
      CONFIGURATIONS Debug
    )
    file (GLOB _files "${CMAKE_SOURCE_DIR}/WIN/DLLs/DLLs 32 release/*.dll")
    install (FILES ${_files}
      DESTINATION ${OPENFLIPPER_LIBDIR}
      CONFIGURATIONS Release
    )
  endif ()
elseif (APPLE)
  set (CPACK_GENERATOR "PackageMaker;TGZ")
  set (CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/OpenFlipper/Icons/OpenFlipper_Icon.icns")
  set (CPACK_PACKAGING_INSTALL_PREFIX "/")
  set (CPACK_PACKAGE_DEFAULT_LOCATION "/Applications")
endif ()

# has to be last
include (CPack)

if (NOT WIN32 AND NOT APPLE)
  # no binary target for linux
  file (REMOVE "${CMAKE_BINARY_DIR}/CPackConfig.cmake")
endif ()