# - Try to find LRI
# Once done this will define
#  
#  LRI_FOUND        - system has LRI
#  LRI_INCLUDE_DIR  - the LRI include directory
#  LRI_LIBRARY_DIR  - where the libraries are
#  LRI_LIBRARY      - Link these to use LRI
#   

IF (LRI_INCLUDE_DIR)
  # Already in cache, be silent
  SET(LRI_FIND_QUIETLY TRUE)
ENDIF (LRI_INCLUDE_DIR)

FIND_PATH( LRI_INCLUDE_DIR LRI/LRI_Defines.hh
           PATHS "${CMAKE_SOURCE_DIR}/libs/" )

add_definitions (-DLRIDLL -DUSELRI )

IF (LRI_INCLUDE_DIR)
  SET(LRI_FOUND TRUE)
  SET( LRI_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_LIBDIR}" )
  SET( LRI_LIBRARY "LRI")
ELSE (LRI_INCLUDE_DIR)
  SET( LRI_FOUND FALSE )
  SET( LRI_LIBRARY_DIR )
ENDIF (LRI_INCLUDE_DIR)

