
if(BUILD_TESTING)

  if ( NOT QT5_FOUND AND NOT FORCE_QT4)
    find_package(Qt5Core)
  elseif ( NOT QT5_FOUND AND NOT QT4_FOUND)
    find_package (Qt4 COMPONENTS QtCore )
  endif()

  add_executable (compareTool compareTool.cc )

  target_link_libraries (compareTool
    ${QT_LIBRARIES}
  )

  set_target_properties ( compareTool PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OPENFLIPPER_TEST_BINARIES}"
    LIBRARY_OUTPUT_DIRECTORY "${OPENFLIPPER_TEST_BINARIES}"
  )

  if (WIN32)
    add_custom_command (TARGET compareTool POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E
                        copy_if_different
                          ${OPENFLIPPER_TEST_BINARIES}/${CMAKE_CFG_INTDIR}/compareTool.exe
                          ${OPENFLIPPER_TEST_BINARIES}/compareTool.exe)
  endif()

endif()
