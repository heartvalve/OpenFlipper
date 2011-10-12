if (SUPERLU_INCLUDE_DIR)
  # in cache already
  set(SUPERLU_FOUND TRUE)
else (SUPERLU_INCLUDE_DIR)

if (WIN32)

   find_path(SUPERLU_INCLUDE_DIR NAMES SRC/supermatrix.h
     PATHS
     "C:\\libs\\SuperLU_3.0"
     ${SUPERLU_INCLUDE_PATH}
   )

   find_library( SUPERLU_LIBRARY_RELEASE 
                 SuperLU
                 PATHS "C:\\libs\\SuperLU_3.0\\lib" )
   find_library( SUPERLU_LIBRARY_DEBUG
                   SuperLUd
                   PATHS "C:\\libs\\SuperLU_3.0\\lib" )


   set ( SUPERLU_LIBRARY "optimized;${SUPERLU_LIBRARY_RELEASE};debug;${SUPERLU_LIBRARY_DEBUG}" CACHE  STRING "SuperLU Libraries" )

ELSEIF(APPLE)

   find_path(SUPERLU_INCLUDE_DIR NAMES supermatrix.h
	     PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/SuperLU_3.0.64-bit.snow_leopard/SuperLU"
	     	   ${SUPERLU_INCLUDE_PATH}
            )

   find_library( SUPERLU_LIBRARY 
                 SuperLU
                 PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/SuperLU_3.0.64-bit.snow_leopard/SuperLU")

ELSE( WIN32 )
   find_path(SUPERLU_INCLUDE_DIR NAMES supermatrix.h
     PATHS /usr/include/superlu
     ${SUPERLU_INCLUDE_PATH}
   )

   find_library( SUPERLU_LIBRARY 
                 superlu
                 PATHS /usr/lib )

ENDIF()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SUPERLU DEFAULT_MSG SUPERLU_INCLUDE_DIR )

endif(SUPERLU_INCLUDE_DIR)

