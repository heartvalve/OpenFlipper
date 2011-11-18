
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
  ADD_TEST(SmootherTest ${OPENFLIPPER_EXECUTABLE} -b  )

  # Timeout after 60 seconds if we havean endless loop
  # Should be run serial to avoid collisons with other instances
  # Only one processor required
  set_tests_properties (
      SmootherTest PROPERTIES
      TIMEOUT 60
      RUN_SERIAL TRUE
      PROCESSORS 1
  ) 

#  set( test_cmd ${OPENFLIPPER_EXECUTABLE} )
#  set( args "-b -c" )
#  add_test( createExprTest_1
#     ${CMAKE_COMMAND}
#     -D test_cmd=${test_cmd}
#     -D test_args:string=${args}
#     -D output_blessed=${CMAKE_SOURCE_DIR}/util/test/test1.h
#     -D output_test=${CMAKE_BINARY_DIR}/util/test1.h
#     -P ${CMAKE_SOURCE_DIR}/tests/run_test_compare.cmake
#  )


  set( test_cmd ${OPENFLIPPER_EXECUTABLE} )
  set( args "-c -b smoother_test_script.ofs" )
  add_test( SmootherResult 
     ${CMAKE_COMMAND}
     -D test_cmd=${test_cmd}
     -D test_args:string=${args}
     -D md5=d41d8cd98f00b204e9800998ecf8427e
     -D output_test=${OPENFLIPPER_TEST_RESULTS}/smoother_test_result.obj
     -P ${CMAKE_SOURCE_DIR}/tests/run_test_md5.cmake
  )






endif()
