
if(BUILD_TESTING)
  
  add_executable (test_example  test_example.cc )
  
  set_target_properties (
      test_example PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
	
  if (WIN32)
    add_custom_command (TARGET test_example POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E
                        copy_if_different
                          ${CMAKE_BINARY_DIR}/tests/${CMAKE_CFG_INTDIR}/test_example.exe
                          ${CMAKE_BINARY_DIR}/tests/test_example.exe)
  endif()

  ADD_TEST(NAME "Test_Example"
           COMMAND ${CMAKE_BINARY_DIR}/tests/test_example )

  set_tests_properties( 
    Test_Example PROPERTIES
    TIMEOUT 10
   )

endif()
