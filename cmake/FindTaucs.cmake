# - Find TAUCS
# Find the native TAUCS headers and libraries.
#
#  TAUCS_INCLUDE_DIR -  where to find TAUCS.h, etc.
#  TAUCS_LIBRARIES    - List of libraries when using TAUCS.
#  TAUCS_FOUND        - True if TAUCS found.

MESSAGE(STATUS "TAUCS finder under DEVELOPMENT!!!")
IF (TAUCS_INCLUDE_DIR)
  # Already in cache, be silent
  SET(TAUCS_FIND_QUIETLY TRUE)
ENDIF (TAUCS_INCLUDE_DIR)


# Look for the header file.
FIND_PATH(TAUCS_INCLUDE_DIR NAMES taucs.h
                      PATHS  /ACG/acgdev/gcc-4.3-x86_64/taucs-2.2-gfortran/
                      )

#

# Copy the results to the output variables.
IF(TAUCS_INCLUDE_DIR )
  SET(TAUCS_FOUND 1)
  SET(TAUCS_INCLUDE_DIR ${TAUCS_INCLUDE_DIR})

  FIND_LIBRARY( TAUCS_LIBRARY
                             NAMES taucs.a
                             PATHS /ACG/acgdev/gcc-4.3-x86_64/taucs-2.2.new/lib )
ELSE(TAUCS_INCLUDE_DIR )
  SET(TAUCS_FOUND 0)
  SET(TAUCS_INCLUDE_DIR)
ENDIF(TAUCS_INCLUDE_DIR )

# Report the results.
IF(NOT TAUCS_FOUND)
  SET(TAUCS_DIR_MESSAGE
    "TAUCS was not found. Make sure TAUCS_INCLUDE_DIR is set to the directories containing the include and lib files for TAUCS. .")
  IF(TAUCS_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "${TAUCS_DIR_MESSAGE}")
  ELSEIF(NOT TAUCS_FIND_QUIETLY)
    MESSAGE(STATUS "${TAUCS_DIR_MESSAGE}")
  ELSE(NOT TAUCS_FIND_QUIETLY)
  ENDIF(TAUCS_FIND_REQUIRED)
ELSE (NOT TAUCS_FOUND)
  IF(NOT TAUCS_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for TAUCS - found")
  ENDIF(NOT TAUCS_FIND_QUIETLY)
ENDIF(NOT TAUCS_FOUND)

