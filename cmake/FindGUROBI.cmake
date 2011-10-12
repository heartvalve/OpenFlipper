if (GUROBI_INCLUDE_DIR)
  # in cache already
  set(GUROBI_FOUND TRUE)
else (GUROBI_INCLUDE_DIR)

if (WIN32)
   find_path(GUROBI_INCLUDE_DIR NAMES SRC/gurobi_c++.h
     PATHS
     "C:\\libs\\gurobi45"
     ${GUROBI_INCLUDE_PATH}
   )

   find_library( GUROBI_LIBRARY_RELEASE 
                 SuperLU
                 PATHS "C:\\libs\\gurobi45\\lib" )
   find_library( GUROBI_LIBRARY_DEBUG
                   SuperLUd
                   PATHS "C:\\libs\\gurobi45\\lib" )


   set ( GUROBI_LIBRARY "optimized;${GUROBI_LIBRARY_RELEASE};debug;${GUROBI_LIBRARY_DEBUG}" CACHE  STRING "GUROBI Libraries" )

ELSEIF(APPLE)

   find_path(GUROBI_INCLUDE_DIR NAMES gurobi_c++.h
	     PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/gurobi40"
	     	   ${GUROBI_INCLUDE_PATH}
            )

   find_library( GUROBI_LIBRARY 
                 SuperLU
                 PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/gurobi40")

ELSE( WIN32 )
   find_path(GUROBI_INCLUDE_DIR NAMES gurobi_c++.h
     PATHS "$ENV{GUROBI_HOME}/include"
     ${GUROBI_INCLUDE_PATH}
   )

#   MESSAGE(STATUS "$ENV{GUROBI_HOME}/include")
   IF(GUROBI_INCLUDE_DIR)
      SET(GUROBI_FOUND TRUE)
      SET(GUROBI_INCLUDE_DIR ${GUROBI_INCLUDE_DIR})
      SET(GUROBI_LIBRARY_DIR "$ENV{GUROBI_HOME}/lib/" CACHE PATH "Path to GUROBI Library")
      SET(GUROBI_LIBRARY "gurobi45;gurobi_c++;pthread" CACHE STRING "GUROBI Libraries")  
      MESSAGE(STATUS "${GUROBI_LIBRARY_DIR}")
      MESSAGE(STATUS "${GUROBI_LIBRARY}")
    ELSE(GUROBI_INCLUDE_DIR)
      SET(GUROBI_FOUND FALSE)
      SET(GUROBI_INCLUDE_DIR ${GUROBI_INCLUDE_DIR})
    ENDIF(GUROBI_INCLUDE_DIR)

   #find_library( GUROBI_LIBRARY 
    #             gurobi
     #            PATHS "${GUROBI_HOME}/lib" )
ENDIF()


endif(GUROBI_INCLUDE_DIR)