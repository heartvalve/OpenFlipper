include (ACGDoxygen)

IF (DOXYGEN_FOUND)

  set(developer_html_doc_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/DeveloperHTML")
  set(developer_qt_help_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help")

  # Create directories in order to avoid doxygen warnings
  if(NOT (IS_DIRECTORY ${developer_html_doc_dir}) )
     file(MAKE_DIRECTORY ${developer_html_doc_dir} )
  endif()

  # Create directories in order to avoid doxygen warnings
  if(NOT (IS_DIRECTORY ${developer_qt_help_dir}) )
     file(MAKE_DIRECTORY ${developer_qt_help_dir} )
  endif()             

  # use the doxyfile here and generate a documentation target
  # Use the local doxygen config
  # and automatically append to doc target
  acg_create_doc_target( doc-DeveloperHelp )
 
  # install the DeveloperHelp on install
  install(DIRECTORY "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/DeveloperHTML" DESTINATION "${ACG_PROJECT_DATADIR}/Doc" )

ENDIF(DOXYGEN_FOUND)
