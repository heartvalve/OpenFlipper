# - Try to find CHOLMOD
# Once done this will define
#  
#  CHOLMOD_FOUND        - system has CHOLMOD
#  CHOLMOD_INCLUDE_DIRS - the CHOLMOD include directory
#  CHOLMOD_LIBRARY      - Link these to use CHOLMOD
#   

IF (CHOLMOD_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(CHOLMOD_FIND_QUIETLY TRUE)
ENDIF (CHOLMOD_INCLUDE_DIRS)

if( WIN32 )
   FIND_PATH( CHOLMOD_INCLUDE_DIRS cholmod.h
              PATHS "C:\\SuiteSparse\\CHOLMOD\\Include"  )
   FIND_PATH( CHOLMOD_LIBRARY 
		     libcholmod.a
                 PATHS "C:\\SuiteSparse\\CHOLMOD\\Lib" )
   SET( CHOLMOD_LIBRARY "${CHOLMOD_LIBRARY}/libcholmod.a" )
   message(STATUS ${CHOLMOD_LIBRARY})
else( WIN32 )
   FIND_PATH( CHOLMOD_INCLUDE_DIRS cholmod.h
              PATHS /usr/local/include /usr/include /usr/include/suitesparse/
              PATH_SUFFIXES cholmod/ CHOLMOD/ )
   SET( CHOLMOD_NAMES cholmod CHOLMOD )
   FIND_LIBRARY( CHOLMOD_LIBRARY
                 NAMES ${CHOLMOD_NAMES}
                 PATHS /usr/lib /usr/local/lib )
endif( WIN32 )

GET_FILENAME_COMPONENT( CHOLMOD_LIBRARY_DIR ${CHOLMOD_LIBRARY} PATH )

IF (CHOLMOD_INCLUDE_DIRS AND CHOLMOD_LIBRARY)
   IF(WIN32)
    list (APPEND CHOLMOD_INCLUDE_DIRS ${CHOLMOD_INCLUDE_DIRS}/../../UFconfig )
   ENDIF(WIN32)
   SET(CHOLMOD_FOUND TRUE)
ELSE (CHOLMOD_INCLUDE_DIRS AND CHOLMOD_LIBRARY)
   SET( CHOLMOD_FOUND FALSE )
ENDIF (CHOLMOD_INCLUDE_DIRS AND CHOLMOD_LIBRARY)

