include (ACGDoxygen)

IF (DOXYGEN_FOUND)

    set(developer_html_doc_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help/html")
  set(developer_qt_help_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help")

  # Create directories in order to avoid doxygen warnings
  if(NOT (IS_DIRECTORY ${developer_qt_help_dir}) )
     file(MAKE_DIRECTORY ${developer_qt_help_dir} )
  endif()             
  
  # Create directories in order to avoid doxygen warnings
  if(NOT (IS_DIRECTORY ${developer_html_doc_dir}) )
     file(MAKE_DIRECTORY ${developer_html_doc_dir} )
  endif()

  # use the doxyfile here and generate a documentation target
  # Use the local doxygen config
  # and automatically append to doc target
  acg_create_doc_target( doc-DeveloperQtHelp )
 
  # Remove directory with temporary html files after build
  #add_custom_command(TARGET doc-DeveloperQtHelp POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove_directory ${developer_html_doc_dir})

  # On apple we take the whole directory, so we don't need this extra install command
  if ( NOT APPLE )  
    # install the DeveloperQtHelp on install
    install(DIRECTORY "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help" DESTINATION "${ACG_PROJECT_DATADIR}" )
  endif()

ENDIF(DOXYGEN_FOUND)
