# - Try to find LIBXML2
# Once done this will define
#  
#  LIBXML2_FOUND        - system has LIBXML2
#  LIBXML2_INCLUDE_DIR  - the LIBXML2 include directory
#  LIBXML2_LIBRARY      - Link these to use LIBXML2
#   

IF (LIBXML2_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(LIBXML2_FIND_QUIETLY TRUE)
ENDIF (LIBXML2_INCLUDE_DIRS)

FIND_PATH( LIBXML2_INCLUDE_DIR libxml/xmlreader.h
           PATHS "/usr/include/libxml2" )

 FIND_LIBRARY( LIBXML2_LIBRARY
               NAMES xml2
               PATHS /usr/lib /usr/local/lib )


IF (LIBXML2_INCLUDE_DIR AND LIBXML2_LIBRARY)
  SET(LIBXML2_FOUND TRUE)
ELSE (LIBXML2_INCLUDE_DIR AND LIBXML2_LIBRARY)
  SET( LIBXML2_FOUND FALSE )
ENDIF (LIBXML2_INCLUDE_DIR AND LIBXML2_LIBRARY)

