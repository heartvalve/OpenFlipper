
if(BUILD_TESTING)
  
  add_executable (minimalExecutionTest  minimalExecutionTest.cc )
  
  set_target_properties( minimalExecutionTest PROPERTIES
      RUNTIME_OUTPUT_NAME "minimalExecTest"
      RUNTIME_OUTPUT_DIRECTORY "${OPENFLIPPER_TEST_BINARIES}"
      LIBRARY_OUTPUT_DIRECTORY "${OPENFLIPPER_TEST_BINARIES}"
    )
	
  if (WIN32)
    add_custom_command (TARGET minimalExecutionTest POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E
                        copy_if_different
                          ${OPENFLIPPER_TEST_BINARIES}/${CMAKE_CFG_INTDIR}/minimalExecTest.exe
                          ${OPENFLIPPER_TEST_BINARIES}/minimalExecTest.exe)
  endif()

  ADD_TEST(NAME "Minimal_Execution_Test"
           COMMAND ${OPENFLIPPER_TEST_BINARIES}/minimalExecTest )

  set_tests_properties( Minimal_Execution_Test PROPERTIES
    TIMEOUT 10
   )

endif()
