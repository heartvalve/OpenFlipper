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
              PATHS "C:\\libs\\win32\\SuiteSparse\\Include"  )

   # Add cholmod include directory to collection include directories
   IF ( CHOLMOD_INCLUDE_DIR )
	list ( APPEND SUITESPARSE_INCLUDE_DIRS ${CHOLMOD_INCLUDE_DIR} )
   ENDIF( CHOLMOD_INCLUDE_DIR )


   # find path suitesparse library
   FIND_PATH( SUITESPARSE_LIBRARY_DIRS 
	         amd.lib
               PATHS "C:\\libs\\win32\\SuiteSparse\\libs" )

   # if we found the library, add it to the defined libraries
   IF ( SUITESPARSE_LIBRARY_DIRS )
	list ( APPEND SUITESPARSE_LIBRARIES optimized;amd;optimized;camd;optimized;ccolamd;optimized;cholmod;optimized;colamd;optimized;metis;debug;amdd;debug;camdd;debug;ccolamdd;debug;cholmodd;debug;colamdd;debug;metisd;optimized;blas;optimized;libf2c;optimized;lapack;debug;blasd;debug;libf2cd;debug;lapackd )
   ENDIF( SUITESPARSE_LIBRARY_DIRS )  

else( WIN32 )
   IF( APPLE)
	   FIND_PATH( CHOLMOD_INCLUDE_DIR cholmod.h
        	      PATHS  /opt/local/include/ufsparse )

           FIND_PATH( SUITESPARSE_LIBRARY_DIR
                      NAMES libcholmod.a 
                      PATHS /opt/local/lib )

	  

   ELSE(APPLE)
	   FIND_PATH( CHOLMOD_INCLUDE_DIR cholmod.h
        	      PATHS /usr/local/include /usr/include /usr/include/suitesparse/ ${CMAKE_SOURCE_DIR}/MacOS/Libs/cholmod
              	      PATH_SUFFIXES cholmod/ CHOLMOD/ )

   	
           FIND_PATH( SUITESPARSE_LIBRARY_DIR
                      NAMES libcholmod.so 
                      PATHS /usr/lib /usr/local/lib )


   ENDIF(APPLE)

   # Add cholmod include directory to collection include directories
   IF ( CHOLMOD_INCLUDE_DIR )
	list ( APPEND SUITESPARSE_INCLUDE_DIRS ${CHOLMOD_INCLUDE_DIR} )
   ENDIF( CHOLMOD_INCLUDE_DIR )


   # if we found the library, add it to the defined libraries
   IF ( SUITESPARSE_LIBRARY_DIR )
       list ( APPEND SUITESPARSE_LIBRARY_DIRS ${SUITESPARSE_LIBRARY_DIR} )
       FIND_LIBRARY( CHOLMOD_LIBRARY
                     NAMES cholmod CHOLMOD
                     PATHS ${SUITESPARSE_LIBRARY_DIR} )

	IF(APPLE)
		list ( APPEND SUITESPARSE_LIBRARIES ${SUITESPARSE_LIBRARY_DIR}/libamd.a 
                                                    ${SUITESPARSE_LIBRARY_DIR}/libbtf.a
                                                    ${SUITESPARSE_LIBRARY_DIR}/libcamd.a
                                                    ${SUITESPARSE_LIBRARY_DIR}/libccolamd.a
                                                    ${SUITESPARSE_LIBRARY_DIR}/libcolamd.a
                                                    ${SUITESPARSE_LIBRARY_DIR}/libcsparse.a
                                                    ${SUITESPARSE_LIBRARY_DIR}/libcxsparse.a
                                                    ${SUITESPARSE_LIBRARY_DIR}/libklu.a
                                                    ${SUITESPARSE_LIBRARY_DIR}/libspqr.a
                                                    ${SUITESPARSE_LIBRARY_DIR}/libumfpack.a
                                                    ${SUITESPARSE_LIBRARY_DIR}/libmetis.a
			)
	ENDIF(APPLE)


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

