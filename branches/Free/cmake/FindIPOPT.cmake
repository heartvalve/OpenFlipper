if (IPOPT_INCLUDE_DIR)
  # in cache already
  SET(IPOPT_FIND_QUIETLY TRUE)
endif (IPOPT_INCLUDE_DIR)

if (WIN32)
   find_path(IPOPT_INCLUDE_DIR NAMES tao.h
     PREFIXES SRC
     PATHS
     "C:\\libs\\gurobi45"
     ${IPOPT_DIR}/include
   )

   find_library( IPOPT_LIBRARY_RELEASE 
                 SuperLU
                 PATHS "C:\\libs\\gurobi45\\lib" )
   find_library( IPOPT_LIBRARY_DEBUG
                   SuperLUd
                   PATHS "C:\\libs\\gurobi45\\lib" )


   set ( IPOPT_LIBRARY "optimized;${IPOPT_LIBRARY_RELEASE};debug;${IPOPT_LIBRARY_DEBUG}" CACHE  STRING "IPOPT Libraries" )

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
