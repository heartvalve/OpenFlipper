# - Try to find PCRE
# Once done this will define
#  
#  PCRE_FOUND        - system has PCRE
#  PCRE_INCLUDE_DIR  - the PCRE include directory
#  PCRE_LIBRARY      - Link these to use PCRE
#   

IF (PCRE_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(PCRE_FIND_QUIETLY TRUE)
ENDIF (PCRE_INCLUDE_DIRS)

FIND_PATH( PCRE_INCLUDE_DIR pcre.h
           PATHS "/usr/include" )

 FIND_LIBRARY( PCRE_LIBRARY
               NAMES pcre
               PATHS /lib /usr/lib /usr/local/lib )


IF (PCRE_INCLUDE_DIR AND PCRE_LIBRARY)
  SET(PCRE_FOUND TRUE)
ELSE (PCRE_INCLUDE_DIR AND PCRE_LIBRARY)
  SET( PCRE_FOUND FALSE )
ENDIF (PCRE_INCLUDE_DIR AND PCRE_LIBRARY)

