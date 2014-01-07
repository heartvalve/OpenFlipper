# - Try to find OpenVolumeMesh
# Once done this will define
#  
# QJSON_FOUND           - system has QJSON
# QJSON_INCLUDE_DIR     - the QJSON include directory
# QJSON_LIBRARY         - Link these to use QJSON
# QJSON_LIBRARY_DIR     - Library DIR of QJSON
#   

IF (QJSON_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(QJSON_FIND_QUIETLY TRUE)
ENDIF (QJSON_INCLUDE_DIRS)

FIND_PATH(QJSON_INCLUDE_DIR QJson/Serializer
	  PATHS /usr/local/include 
                /usr/include 
                /usr/local/OpenVolumeMesh/include
                "${CMAKE_SOURCE_DIR}/libs_required/qjson/include"
		    "C:\\Program Files\\QJson\\include"
                )

IF (QJSON_INCLUDE_DIR )
   SET(QJSON_FOUND TRUE)
   ADD_DEFINITIONS(-DENABLE_QJSON)
IF (WIN32)
	IF(EXISTS "${QJSON_INCLUDE_DIR}/../lib")
		SET(QJSON_LIBRARY_DIR "${QJSON_INCLUDE_DIR}/../lib")
	ENDIF()
ELSE (WIN32)
	IF(EXISTS "${QJSON_INCLUDE_DIR}/../lib/QJson")
   		SET(QJSON_LIBRARY_DIR "${QJSON_INCLUDE_DIR}/../lib/QJson")
	ENDIF()
ENDIF (WIN32)
   SET(QJSON_LIBRARY "QJson")
ELSE (QJSON_INCLUDE_DIR)
   SET(QJSON_FOUND FALSE )
ENDIF (QJSON_INCLUDE_DIR )

