# - Try to find GEOMETRYLIB
# Once done this will define
#  
#  GEOMETRYLIB_FOUND        - system has GEOMETRYLIB
#  GEOMETRYLIB_INCLUDE_DIR  - the GEOMETRYLIB include directory
#  GEOMETRYLIB_LIBRARY_DIR  - where the libraries are
#  GEOMETRYLIB_LIBRARY      - Link these to use GEOMETRYLIB
#   

IF (GEOMETRYLIB_INCLUDE_DIR)
  # Already in cache, be silent
  SET(GEOMETRYLIB_FIND_QUIETLY TRUE)
ENDIF (GEOMETRYLIB_INCLUDE_DIR)

FIND_PATH( GEOMETRYLIB_INCLUDE_DIR 
           NAMES GeometryLib/Config/GeometryLibDefines.hh
           PATHS /data1/home1/zimmer/projects/libs/ 
                 "${CMAKE_SOURCE_DIR}/libs/" 
                 "${CMAKE_SOURCE_DIR}/.." 
                 ../libs/
                 ./
#                 ~/projects/libs/
                 )

add_definitions (-DGEOMETRYLIBDLL -DUSEGEOMETRYLIB )

IF (GEOMETRYLIB_INCLUDE_DIR)
  SET(GEOMETRYLIB_FOUND TRUE)
  SET( GEOMETRYLIB_LIBRARY_DIR "${ACG_PROJECT_LIBDIR}" )
  SET( GEOMETRYLIB_LIBRARY "GeometryLib")
ELSE (GEOMETRYLIB_INCLUDE_DIR)
  SET( GEOMETRYLIB_FOUND FALSE )
  SET( GEOMETRYLIB_LIBRARY_DIR )
ENDIF (GEOMETRYLIB_INCLUDE_DIR)

