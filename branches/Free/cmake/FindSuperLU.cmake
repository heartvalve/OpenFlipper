if (SUPERLU_INCLUDE_DIR)
  # in cache already
  set(SUPERLU_FOUND TRUE)
else (SUPERLU_INCLUDE_DIR)

find_path(SUPERLU_INCLUDE_DIR NAMES supermatrix.h
     PATHS
     /usr/include/superlu
     ${SUPERLU_INCLUDE_PATH}
   )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SUPERLU DEFAULT_MSG SUPERLU_INCLUDE_DIR )

mark_as_advanced(SUPERLU_INCLUDE_DIR)

endif(SUPERLU_INCLUDE_DIR)

