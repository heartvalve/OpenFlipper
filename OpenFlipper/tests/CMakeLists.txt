
# Include Testing package
INCLUDE(CTest)


if(BUILD_TESTING)

  # ========================================================================
  # Copy converter files from ctest output to junit output
  # ========================================================================
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTest2JUnit.py
    ${CMAKE_BINARY_DIR}/tests/CTest2JUnit.py COPYONLY)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTest2JUnit.xsl
    ${CMAKE_BINARY_DIR}/tests//CTest2JUnit.xsl COPYONLY)




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
  ADD_TEST(BatchStart ${CMAKE_BINARY_DIR}/Build/bin/OpenFlipper -b -c )

  set_tests_properties (
      BatchStart PROPERTIES
      TIMEOUT 1
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

  

endif()
