include (ACGDoxygen)

IF (DOXYGEN_FOUND)

#  if (TARGET doc)
#    acg_copy_after_build (doc "${CMAKE_CURRENT_SOURCE_DIR}/html" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/DeveloperHtml")
#  endif ()

  # use the doxyfile here and generate a documentation target
  acg_create_doc_target( doc-DeveloperHelp )


  # copy acg stylesheet data after build
  if (TARGET doc-DeveloperHelp )
    acg_copy_after_build (doc-DeveloperHelp "${CMAKE_CURRENT_SOURCE_DIR}/../ACGStyleHtml" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/DeveloperHTML/html") 
  endif ()

  # Add a documentation install target
  add_custom_target(doc-Developer-html-install COMMENT "Installing Developer HTML Documentation" VERBATIM )

  # Build Documentation before installing it
  add_dependencies(doc-Developer-html-install doc)

  if (TARGET doc-Developer-html-install)
    acg_copy_after_build (doc-Developer-html-install "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/DeveloperHtml"   "${CMAKE_INSTALL_PREFIX}/${ACG_PROJECT_DATADIR}/Doc/DeveloperHtml")
  endif()

ENDIF(DOXYGEN_FOUND)
