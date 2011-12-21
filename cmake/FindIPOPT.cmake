if (IPOPT_INCLUDE_DIR)
  # in cache already
  SET(IPOPT_FIND_QUIETLY TRUE)
endif (IPOPT_INCLUDE_DIR)

if (WIN32)
   find_path(IPOPT_INCLUDE_DIR NAMES IpNLP.hpp
     PATHS
     "C:\\libs\\Ipopt-3.8.2\\include\\coin"
     ${IPOPT_DIR}/include
   )

   IF(IPOPT_INCLUDE_DIR)
      find_library( IPOPT_LIBRARY_RELEASE 
                    Ipopt
                    PATHS "C:\\libs\\Ipopt-3.8.2\\lib\\win32\\release" )
      find_library( IPOPT_LIBRARY_DEBUG
                    Ipopt
                    PATHS "C:\\libs\\Ipopt-3.8.2\\lib\\win32\\debug" )

      set ( IPOPT_LIBRARY "optimized;${IPOPT_LIBRARY_RELEASE};debug;${IPOPT_LIBRARY_DEBUG}" CACHE  STRING "IPOPT Libraries" )

      SET(IPOPT_FOUND TRUE)
      SET(IPOPT_INCLUDE_DIR ${IPOPT_INCLUDE_DIR})
	  # Todo, set right version depending on build type (debug/release)
	  #GET_FILENAME_COMPONENT( IPOPT_LIBRARY_DIR ${GLEW_LIBRARY} PATH )
    ELSE(IPOPT_INCLUDE_DIR)
      SET(IPOPT_FOUND FALSE)
      SET(IPOPT_INCLUDE_DIR ${IPOPT_INCLUDE_DIR})
    ENDIF(IPOPT_INCLUDE_DIR)

ELSEIF(APPLE)

   find_path(IPOPT_INCLUDE_DIR NAMES gurobi_c++.h
	     PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/gurobi40"
	     	   ${IPOPT_INCLUDE_PATH}
            )

   find_library( IPOPT_LIBRARY 
                 SuperLU
                 PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/gurobi40")

ELSE( WIN32 )
   find_path(IPOPT_INCLUDE_DIR NAMES IpNLP.hpp
     PATHS "/usr/include/coin"
     ${IPOPT_INCLUDE_PATH}
   )

#   MESSAGE(STATUS "$ENV{IPOPT_HOME}/include")
   IF(IPOPT_INCLUDE_DIR)
      SET(IPOPT_FOUND TRUE)
      SET(IPOPT_INCLUDE_DIR ${IPOPT_INCLUDE_DIR})
      
      #wrong config under Debian workaround
      add_definitions( -DHAVE_CSTDDEF )
#      SET(IPOPT_LIBRARY_DIR "$ENV{IPOPT_DIR}/lib/$ENV{IPOPT_ARCH}/" CACHE PATH "Path to IPOPT Library")
#      SET(IPOPT_LIBRARY "tao;taopetsc;taofortran" CACHE STRING "IPOPT Libraries")  
#      MESSAGE(STATUS "${IPOPT_LIBRARY_DIR}")
#      MESSAGE(STATUS "${IPOPT_LIBRARY}")
    ELSE(IPOPT_INCLUDE_DIR)
      SET(IPOPT_FOUND FALSE)
      SET(IPOPT_INCLUDE_DIR ${IPOPT_INCLUDE_DIR})
    ENDIF(IPOPT_INCLUDE_DIR)

   find_library( IPOPT_LIBRARY 
                 ipopt
                 PATHS "/usr/lib" )
   
   # set optional path to HSL Solver
   find_path(IPOPT_HSL_LIBRARY_DIR NAMES libhsl.so
     PATHS "$ENV{HOME}/opt/HSL/lib"
   )
   
   IF( IPOPT_HSL_LIBRARY_DIR)
     set(IPOPT_LIBRARY_DIR ${IPOPT_HSL_LIBRARY_DIR})
   ENDIF(IPOPT_HSL_LIBRARY_DIR)
ENDIF()
