
# Include Testing package
if(BUILD_TESTING)

  # ========================================================================
  # Copy required files
  # ========================================================================
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/smoother_test_script.ofs
   ${CMAKE_BINARY_DIR}/tests/Plugin-Smoother/smoother_test_script.ofs @ONLY )

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
  ADD_TEST(PLUGIN-SMOOTHER-NOGUI_Start_OpenFlipper_in_batch_mode ${OPENFLIPPER_EXECUTABLE} -b  )

  # Timeout after 60 seconds if we havean endless loop
  # Should be run serial to avoid collisons with other instances
  # Only one processor required
  set_tests_properties (
      PLUGIN-SMOOTHER-NOGUI_Start_OpenFlipper_in_batch_mode PROPERTIES
      TIMEOUT 60
      RUN_SERIAL TRUE
      PROCESSORS 1
  ) 

  set( test_cmd ${OPENFLIPPER_EXECUTABLE} )
  set( args "-c -b smoother_test_script.ofs" )
  add_test( PLUGIN-SMOOTHER-NOGUI_Cube_10_Iterations_tangential_C1 
     ${CMAKE_COMMAND}
     -D test_cmd=${test_cmd}
     -D test_args:string=${args}
     -D md5=23699491b2e4a2517d41ec91acca13db
     -D output_test=${OPENFLIPPER_TEST_RESULTS}/smoother_test_result.obj
     -P ${CMAKE_SOURCE_DIR}/tests/run_test_md5.cmake
  )

  # Timeout after 2 minutes if we have an endless loop
  # Should be run serial to avoid collisons with other instances
  # Only one processor required
  set_tests_properties (
      PLUGIN-SMOOTHER-NOGUI_Cube_10_Iterations_tangential_C1 PROPERTIES
      TIMEOUT 120
      RUN_SERIAL TRUE
      PROCESSORS 1
  )





endif()
