# - Try to find SUITESPARSE
# Once done this will define
#  
#  SUITESPARSE_FOUND        - system has SUITESPARSE
#  SUITESPARSE_INCLUDE_DIRS - the SUITESPARSE include directory
#  SUITESPARSE_LIBRARIES    - Link these to use SUITESPARSE
#  SUITESPARSE_LIBRARY_DIR  - Library directory containing suitesparse libs
#   

IF (SUITESPARSE_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(SUITESPARSE_FIND_QUIETLY TRUE)
ENDIF (SUITESPARSE_INCLUDE_DIRS)

if( WIN32 )
   # Find cholmod part of the suitesparse library collection

   FIND_PATH( CHOLMOD_INCLUDE_DIR cholmod.h
              PATHS "C:\\libs\\SuiteSparse\\CHOLMOD\\Include"  )

   # Add cholmod include directory to collection include directories
   IF ( CHOLMOD_INCLUDE_DIR )
	list ( APPEND SUITESPARSE_INCLUDE_DIRS ${CHOLMOD_INCLUDE_DIR} )
   ENDIF( CHOLMOD_INCLUDE_DIR )

   

   # find path suitesparse library
   FIND_PATH( SUITESPARSE_LIBRARY_DIRS 
	         suitesparse.lib
               PATHS "C:\\libs\\SuiteSparse\\libs" )

   # if we found the library, add it to the defined libraries
   IF ( SUITESPARSE_LIBRARY_DIRS )
	list ( APPEND SUITESPARSE_LIBRARIES suitesparse )
   ENDIF( SUITESPARSE_LIBRARY_DIRS )  

else( WIN32 )
   FIND_PATH( CHOLMOD_INCLUDE_DIR cholmod.h
              PATHS /usr/local/include /usr/include /usr/include/suitesparse/
              PATH_SUFFIXES cholmod/ CHOLMOD/ )

   # Add cholmod include directory to collection include directories
   IF ( CHOLMOD_INCLUDE_DIR )
	list ( APPEND SUITESPARSE_INCLUDE_DIRS ${CHOLMOD_INCLUDE_DIR} )
   ENDIF( CHOLMOD_INCLUDE_DIR )

   FIND_PATH( SUITESPARSE_LIBRARY_DIR
              NAMES libcholmod.so 
              PATHS /usr/lib /usr/local/lib )

   # if we found the library, add it to the defined libraries
   IF ( SUITESPARSE_LIBRARY_DIR )
       FIND_LIBRARY( CHOLMOD_LIBRARY
                     NAMES cholmod CHOLMOD
                     PATHS ${SUITESPARSE_LIBRARY_DIR} )
	list ( APPEND SUITESPARSE_LIBRARIES ${CHOLMOD_LIBRARY} )
   ENDIF( SUITESPARSE_LIBRARY_DIR )  
   
endif( WIN32 )


IF (SUITESPARSE_INCLUDE_DIRS AND SUITESPARSE_LIBRARIES)
   IF(WIN32)
    list (APPEND SUITESPARSE_INCLUDE_DIRS ${CHOLMOD_INCLUDE_DIR}/../../UFconfig )
   ENDIF(WIN32)
   SET(SUITESPARSE_FOUND TRUE)
ELSE (SUITESPARSE_INCLUDE_DIRS AND SUITESPARSE_LIBRARIES)
   SET( SUITESPARSE_FOUND FALSE )
ENDIF (SUITESPARSE_INCLUDE_DIRS AND SUITESPARSE_LIBRARIES)

