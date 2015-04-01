#
# Try to find MLPACK
# Once done this will define
#
# MLPACK_FOUND           - system has MLPACK
# MLPACK_INCLUDE_DIRS    - the MLPACK include directories
# MLPACK_LIBRARIES       - Link these to use MLPACK

IF (MLPACK_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(MLPACK_FIND_QUIETLY TRUE)
ENDIF (MLPACK_INCLUDE_DIRS)

FIND_PATH(MLPACK_INCLUDE_DIR core.hpp
	  PATHS /usr/local/include/mlpack
                /usr/include/mlpack
         )

SET(MLPACK_LIBRARY_DIR NOTFOUND CACHE PATH "The directory where the MLPACK libraries can be found.")
SET(SEARCH_PATHS
    "${MLPACK_INCLUDE_DIR}/../lib"
    "${MLPACK_INCLUDE_DIR}/../../lib"
    "${MLPACK_LIBRARY_DIR}")
FIND_LIBRARY(MLPACK_LIBRARY NAMES mlpack PATHS ${SEARCH_PATHS})

INCLUDE (FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(mlpack DEFAULT_MSG MLPACK_LIBRARY MLPACK_INCLUDE_DIR)

IF (MLPACK_FOUND)
   SET(MLPACK_LIBRARIES "${MLPACK_LIBRARY}")
   SET(MLPACK_INCLUDE_DIRS "${MLPACK_INCLUDE_DIR}")
ENDIF (MLPACK_FOUND)
