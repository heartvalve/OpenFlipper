# - Try to find ISOEX
# Once done this will define
#  
#  ISOEX_FOUND        - system has ISOEX
#  ISOEX_INCLUDE_DIR  - the ISOEX include directory
#  ISOEX_LIBRARY_DIR  - where the libraries are
#  ISOEX_LIBRARY      - Link these to use ISOEX
#   

IF (ISOEX_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(ISOEX_FIND_QUIETLY TRUE)
ENDIF (ISOEX_INCLUDE_DIRS)

FIND_PATH( ISOEX_INCLUDE_DIRS IsoEx/Extractors/MarchingCubesT.hh
           PATHS "${CMAKE_SOURCE_DIR}/libs" )

add_definitions (-DISOEXDLL -DUSEISOEX )

IF (ISOEX_INCLUDE_DIRS)
  SET(ISOEX_FOUND TRUE)
  SET( ISOEX_LIBRARY_DIR "${ACG_PROJECT_LIBDIR}" )
  SET( ISOEX_LIBRARY "IsoEx")
  list (APPEND ISOEX_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/libs/IsoEx")
ELSE (ISOEX_INCLUDE_DIRS)
  SET( ISOEX_FOUND FALSE )
  SET( ISOEX_LIBRARY_DIR )
ENDIF (ISOEX_INCLUDE_DIRS)

