# - Find ADOL-C
# Find the native ADOL-C headers and libraries.
#
#  ADOLC_INCLUDE_DIR  - Where to find <adolc/adolc.h>, etc.
#  ADOLC_FOUND        - True if ADOL-C found.
#  ADOLC_LIBRARY_DIR  - Where to find the library files
#  ADOLC_LIBRARIES    - Which libraries are available

IF (ADOLC_INCLUDE_DIR)
  # Already in cache, be silent
  SET(ADOLC_FIND_QUIETLY TRUE)
ENDIF (ADOLC_INCLUDE_DIR)

GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

# Look for the header files
FIND_PATH(ADOLC_INCLUDE_DIR NAMES adolc/adolc.h 
                             PATHS /usr/include/adolc
                                   /usr/local/include
                                   /usr/local/include/adolc/
                                   /opt/local/include/adolc/
                                   ${ADOLC_DIR}/include
                                   "c:\\libs\\adolc\\include"
		                   "c:\\libs\\adolc\\include"
				    ${PROJECT_SOURCE_DIR}/MacOS/Libs/adolc/include
                                   ../../External/include
                                   ${module_file_path}/../../../External/include)


# Copy the results to the output variables.
IF(ADOLC_INCLUDE_DIR )
  SET(ADOLC_FOUND 1)
  SET(ADOLC_INCLUDE_DIR ${ADOLC_INCLUDE_DIR})
ELSE(ADOLC_INCLUDE_DIR )
  SET(ADOLC_FOUND 0)
  SET(ADOLC_INCLUDE_DIR)
ENDIF(ADOLC_INCLUDE_DIR )

# Look for the libraries
find_library(ADOLC_LIBRARIES NAMES adolc 
                   PATHS
                   ${ADOLC_INCLUDE_DIR}/../lib
                   ${ADOLC_INCLUDE_DIR}/../lib64
                   /usr/lib/adolc
                   /usr/local/lib
                   /usr/local/lib/adolc/
                   /opt/local/lib/adolc/
                   "c:\\libs\\adolc\\lib"
                   "c:\\libs\\adolc\\lib"
				    ${PROJECT_SOURCE_DIR}/MacOS/Libs/adolc/lib
				    ${ADOLC_DIR}/include
				    /usr/lib64/adolc
                   /usr/local/lib64
                   /usr/local/lib64/adolc/
                   /opt/local/lib64/adolc/
                   "c:\\libs\\adolc\\lib64"
                   "c:\\libs\\adolc\\lib64"
				    ${PROJECT_SOURCE_DIR}/MacOS/Libs/adolc/lib64
                   DOC "Path to the ADOL-C library"
                  )

get_filename_component(ADOLC_LIBRARY_DIR ${ADOLC_LIBRARIES} PATH CACHE)

# Report the results.
IF(NOT ADOLC_FOUND)
  SET(ADOLC_DIR_MESSAGE
    "ADOLC was not found. Make sure ADOLC_INCLUDE_DIR is set to the directories containing the include and lib files for ADOLC.")
ELSE (NOT ADOLC_FOUND)
  ADD_DEFINITIONS(-DADOLC_AVAILABLE)
  IF(NOT ADOLC_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for ADOLC - found")
  ENDIF(NOT ADOLC_FIND_QUIETLY)
ENDIF(NOT ADOLC_FOUND)

