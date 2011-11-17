
# Include Testing package
INCLUDE(CTest)


if(BUILD_TESTING)

SET (CTEST_DROP_SITE_CDASH FALSE)

  # ========================================================================
  # Copy converter files from ctest output to correctly generate xml output
  # ========================================================================
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/run_tests.sh
    ${CMAKE_BINARY_DIR}/tests/run_tests.sh COPYONLY)

  # ========================================================================
  # ========================================================================
  # Internal global tests start here
  # ========================================================================
  # ========================================================================

  
  # ========================================================================
  # Start OpenFlipper Headless in batch mode, to see if all plugins
  # that pretend to be scriptable without ui are correct and the core
  # can start
  # ========================================================================

  ADD_TEST(BatchStart "${OPENFLIPPER_EXECUTABLE}" -b -c )

  # Timeout after 60 seconds if we havean endless loop
  # Should be run serial to avoid collisons with other instances
  # Only one processor required
  set_tests_properties (
      BatchStart PROPERTIES
      TIMEOUT 60
      RUN_SERIAL TRUE
      PROCESSORS 1
  ) 

  
  # ========================================================================
  # ========================================================================
  # ========================================================================
  # Add tests from subdirectories
  # ========================================================================
  # ========================================================================
  # ========================================================================
  
  # search all libs in the libs directory 
  file (
    GLOB _local_tests_in
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/*/CMakeLists.txt"
  )

  # add all found tests to the build
  foreach (_localTest ${_local_tests_in})
    get_filename_component (_local_test_dir ${_localTest} PATH)
    add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/${_local_test_dir})
  endforeach ()






  # ========================================================================
  # ========================================================================
  # ========================================================================
  # Add tests from plugins
  # ========================================================================
  # ========================================================================
  # ========================================================================


  file (
      GLOB _plugins_in
      RELATIVE "${CMAKE_SOURCE_DIR}"
      "${CMAKE_SOURCE_DIR}/Plugin-*/CMakeLists.txt"
  )

  foreach (_plugin ${_plugins_in})

    get_filename_component (_plugin_dir ${_plugin} PATH)

     # If a test subdir exists, we add it here
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${_plugin_dir}/tests)
      add_subdirectory(${CMAKE_SOURCE_DIR}/${_plugin_dir}/tests ${CMAKE_BINARY_DIR}/tests/${_plugin_dir} )
    endif()

   endforeach ()

endif()
