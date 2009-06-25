if (SUPERLU_INCLUDE_DIR)
  # in cache already
  set(SUPERLU_FOUND TRUE)
else (SUPERLU_INCLUDE_DIR)

if (WIN32)
   find_path(SUPERLU_INCLUDE_DIR NAMES supermatrix.h
     PREFIXES SRC
     PATHS
     "C:\\libs\\SuperLU_3.0"
     ${SUPERLU_INCLUDE_PATH}
   )

   find_library( SUPERLU_LIBRARY 
                 SuperLU
                 PATHS "C:\\libs\\SuperLU_3.0\\lib" )

ELSEIF(APPLE)

   find_path(SUPERLU_INCLUDE_DIR NAMES supermatrix.h
	     PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/SuperLU_3.0/SuperLU"
	     	   ${SUPERLU_INCLUDE_PATH}
            )

   find_library( SUPERLU_LIBRARY 
                 superlu
                 PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/SuperLU_3.0/SuperLU")

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

