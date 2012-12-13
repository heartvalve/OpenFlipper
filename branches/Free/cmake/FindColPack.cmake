# - Find ColPack
# Find the native ColPack headers and libraries.
#
#  COLPACK_INCLUDE_DIR  - Where to find <ColPack/ColPackHeaders.h>, etc.
#  COLPACK_FOUND        - True if ColPack found.
#  COLPACK_LIBRARY_DIR  - Where to find the library files
#  COLPACK_LIBRARIES    - Which libraries are available

IF (COLPACK_INCLUDE_DIR)
  # Already in cache, be silent
  SET(COLPACK_FIND_QUIETLY TRUE)
ENDIF (COLPACK_INCLUDE_DIR)

GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

# Look for the header files
FIND_PATH(COLPACK_INCLUDE_DIR NAMES ColPack/ColPackHeaders.h 
                             PATHS /usr/include/ColPack
                                   /usr/local/include
                                   /usr/local/include/ColPack/
                                   /opt/local/include/ColPack/
                                   ${COLPACK_DIR}/include
                                   "c:\\libs\\ColPack\\include"
		                   "c:\\libs\\ColPack\\include"
				    ${PROJECT_SOURCE_DIR}/MacOS/Libs/ColPack/include
                                   ../../External/include
                                   ${module_file_path}/../../../External/include)


# Copy the results to the output variables.
IF(COLPACK_INCLUDE_DIR )
  SET(COLPACK_FOUND 1)
  SET(COLPACK_INCLUDE_DIR ${COLPACK_INCLUDE_DIR})
ELSE(COLPACK_INCLUDE_DIR )
  SET(COLPACK_FOUND 0)
  SET(COLPACK_INCLUDE_DIR)
ENDIF(COLPACK_INCLUDE_DIR )

# Look for the libraries
find_library(COLPACK_LIBRARIES NAMES ColPack 
                   PATHS
                   ${COLPACK_INCLUDE_DIR}/../lib
                   ${COLPACK_INCLUDE_DIR}/../lib64
                   /usr/lib/ColPack
                   /usr/local/lib
                   /usr/local/lib/ColPack/
                   /opt/local/lib/ColPack/
                   "c:\\libs\\ColPack\\lib"
                   "c:\\libs\\ColPack\\lib"
				    ${PROJECT_SOURCE_DIR}/MacOS/Libs/ColPack/lib
				    ${COLPACK_DIR}/include
				    /usr/lib64/ColPack
                   /usr/local/lib64
                   /usr/local/lib64/ColPack/
                   /opt/local/lib64/ColPack/
                   "c:\\libs\\ColPack\\lib64"
                   "c:\\libs\\ColPack\\lib64"
				    ${PROJECT_SOURCE_DIR}/MacOS/Libs/ColPack/lib64
                   DOC "Path to the ColPack library"
                  )

get_filename_component(COLPACK_LIBRARY_DIR ${COLPACK_LIBRARIES} PATH CACHE)

# Report the results.
IF(NOT COLPACK_FOUND)
  SET(COLPACK_DIR_MESSAGE
    "COLPACK was not found. Make sure COLPACK_INCLUDE_DIR is set to the directories containing the include and lib files for COLPACK.")
ELSE (NOT COLPACK_FOUND)
  ADD_DEFINITIONS(-DCOLPACK_AVAILABLE)
  IF(NOT COLPACK_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for ColPack - found")
  ENDIF(NOT COLPACK_FIND_QUIETLY)
ENDIF(NOT COLPACK_FOUND)

