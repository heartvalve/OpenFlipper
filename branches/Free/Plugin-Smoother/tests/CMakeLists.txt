
# Include Testing package
if(BUILD_TESTING)

  # ========================================================================
  # Copy required files
  # ========================================================================
  #configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTest2JUnit.py
  #  ${CMAKE_BINARY_DIR}/tests/CTest2JUnit.py COPYONLY)


  # ========================================================================
  # ========================================================================
  # Tests
  # ========================================================================
  # ========================================================================

  # ========================================================================
  # Start OpenFlipper Headless in batch mode, to see if all plugins
  # that pretend to be scriptable without ui are correct and the core
  # can start
  # ========================================================================
  ADD_TEST(SmootherTest ${OPENFLIPPER_EXECUTABLE} -b -c )

  # Timeout after 60 seconds if we havean endless loop
  # Should be run serial to avoid collisons with other instances
  # Only one processor required
  set_tests_properties (
      SmootherTest PROPERTIES
      TIMEOUT 60
      RUN_SERIAL TRUE
      PROCESSORS 1
  ) 




endif()
