
if(BUILD_TESTING)
  
  add_executable (minimalExecutionTest  minimalExecutionTest.cc )
  
  set_target_properties( minimalExecutionTest PROPERTIES
      RUNTIME_OUTPUT_NAME "minimalExecTest"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
	
  if (WIN32)
    add_custom_command (TARGET minimalExecutionTest POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E
                        copy_if_different
                          ${CMAKE_BINARY_DIR}/tests/${CMAKE_CFG_INTDIR}/minimalExecTest.exe
                          ${CMAKE_BINARY_DIR}/tests/minimalExecTest.exe)
  endif()

  ADD_TEST(NAME "Minimal_Execution_Test"
           COMMAND ${CMAKE_BINARY_DIR}/tests/minimalExecTest )

  set_tests_properties( Minimal_Execution_Test PROPERTIES
    TIMEOUT 10
   )

endif()
