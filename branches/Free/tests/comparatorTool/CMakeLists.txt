
if(BUILD_TESTING)
 
  find_package (Qt4 COMPONENTS QtCore )
 
  add_executable (comparatorTool comparator.cc )

  target_link_libraries (comparatorTool
    ${QT_LIBRARIES}
  ) 



 
  set_target_properties ( comparatorTool PROPERTIES
    RUNTIME_OUTPUT_NAME "compareTool"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
  )
	
  if (WIN32)
    add_custom_command (TARGET test_example POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E
                        copy_if_different                               
                          ${CMAKE_BINARY_DIR}/tests/${CMAKE_CFG_INTDIR}/compareTool.exe
                          ${CMAKE_BINARY_DIR}/tests/compareTool.exe)
  endif()

endif()
