include (ACGDoxygen)

IF (DOXYGEN_FOUND)

  set(developer_html_doc_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/DeveloperHTML")

  # Create directories in order to avoid doxygen warnings
  if(NOT (IS_DIRECTORY ${developer_html_doc_dir}) )
     file(MAKE_DIRECTORY ${developer_html_doc_dir} )
  endif()

  # use the doxyfile here and generate a documentation target
  # Use the local doxygen config
  # and automatically append to doc target
  acg_create_doc_target( doc-DeveloperHelp )

ENDIF(DOXYGEN_FOUND)
