include (ACGDoxygen)

IF (DOXYGEN_FOUND)

    set(user_html_doc_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help/html")
  set(user_qt_help_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help")

  # Create directories in order to avoid doxygen warnings
  if(NOT (IS_DIRECTORY ${user_html_doc_dir}) )
     file(MAKE_DIRECTORY ${user_html_doc_dir} )
  endif()

  # Create directories in order to avoid doxygen warnings
  if(NOT (IS_DIRECTORY ${user_qt_help_dir}) )
     file(MAKE_DIRECTORY ${user_qt_help_dir} )
  endif()                                     

  # use the doxyfile here and generate a documentation target which builds only the user Help
  # Function defaults to current dir and adds a dependency to the global doc target
  acg_create_doc_target( doc-UserQtHelp )
  
  # Remove temporary html files after buily
  add_custom_command(TARGET doc-UserQtHelp POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove_directory ${user_html_doc_dir})

  # install the UserHelp on install
  # This will also install the plugin User Documentations
  install(DIRECTORY "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help" DESTINATION "${ACG_PROJECT_DATADIR}/Help" )

ENDIF(DOXYGEN_FOUND)
