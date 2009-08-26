# - Try to find COMISO
# Once done this will define
#  
#  COMISO_FOUND        - system has COMISO
#  COMISO_INCLUDE_DIR  - the COMISO include directory
#  COMISO_LIBRARY_DIR  - where the libraries are
#  COMISO_LIBRARY      - Link these to use COMISO
#   

IF (COMISO_INCLUDE_DIR)
  # Already in cache, be silent
  SET(COMISO_FIND_QUIETLY TRUE)
ENDIF (COMISO_INCLUDE_DIR)

FIND_PATH( COMISO_INCLUDE_DIR CoMISo/Config/CoMISoDefines.hh
           PATHS "${CMAKE_SOURCE_DIR}/libs/" )

add_definitions (-DCOMISODLL -DUSECOMISO )

IF (COMISO_INCLUDE_DIR)
  SET(COMISO_FOUND TRUE)
  SET( COMISO_LIBRARY_DIR "${ACG_PROJECT_LIBDIR}" )
  SET( COMISO_LIBRARY "CoMISo")
ELSE (COMISO_INCLUDE_DIR)
  SET( COMISO_FOUND FALSE )
  SET( COMISO_LIBRARY_DIR )
ENDIF (COMISO_INCLUDE_DIR)

