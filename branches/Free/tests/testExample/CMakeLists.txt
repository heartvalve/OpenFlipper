
if(BUILD_TESTING)
  
  add_executable (test_example  test_example.cc )
  
  set_target_properties (
      test_example PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )

  ADD_TEST(NAME "Test_Example"
           COMMAND ${CMAKE_BINARY_DIR}/tests/test_example )

  set_tests_properties( 
    Test_Example PROPERTIES
    TIMEOUT 10
   )

endif()
