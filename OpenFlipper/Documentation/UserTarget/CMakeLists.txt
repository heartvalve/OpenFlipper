include (ACGDoxygen)

IF (DOXYGEN_FOUND)

#  if (TARGET doc)
#    acg_copy_after_build (doc "${CMAKE_CURRENT_SOURCE_DIR}/html" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/DeveloperHtml")
#  endif ()

  # use the doxyfile here and generate a documentation target
  acg_create_doc_target( doc-UserHelp )

  # copy acg stylesheet data after build
  if (TARGET doc-UserHelp )
    acg_copy_after_build (doc-UserHelp "${CMAKE_CURRENT_SOURCE_DIR}/../ACGStyleHtml" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/UserHTML/html") 
  endif ()

  # Add a documentation install target
  add_custom_target(doc-User-html-install COMMENT "Installing User HTML Documentation" VERBATIM )

  # Build Documentation before installing it
  add_dependencies(doc-User-html-install doc)

  if (TARGET doc-Developer-html-install)
    acg_copy_after_build (doc-User-html-install "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/UserHtml"   "${CMAKE_INSTALL_PREFIX}/${ACG_PROJECT_DATADIR}/Doc/UserHtml")
  endif()

ENDIF(DOXYGEN_FOUND)
