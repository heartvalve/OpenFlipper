include (ACGDoxygen)

IF (DOXYGEN_FOUND)

  set(user_html_doc_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/UserHTML")

  # Create directories in order to avoid doxygen warnings
  if(NOT (IS_DIRECTORY ${user_html_doc_dir}) )
     file(MAKE_DIRECTORY ${user_html_doc_dir} )
  endif()

  # use the doxyfile here and generate a documentation target which builds only the user Help
  # Function defaults to current dir and adds a dependency to the global doc target
  acg_create_doc_target( doc-UserHelp )

ENDIF(DOXYGEN_FOUND)
