# - Try to find OpenVolumeMesh
# Once done this will define
#  
# OPENVOLUMEMESH_FOUND           - system has OPENVOLUMEMESH
# OPENVOLUMEMESH_INCLUDE_DIR     - the OPENVOLUMEMESH include directory
# OPENVOLUMEMESH_LIBRARY         - Link these to use OPENVOLUMEMESH
# OPENVOLUMEMESH_LIBRARY_DIR     - Library DIR of OPENVOLUMEMESH
#   

IF (OPENVOLUMEMESH_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(OPENVOLUMEMESH_FIND_QUIETLY TRUE)
ENDIF (OPENVOLUMEMESH_INCLUDE_DIRS)

FIND_PATH(OPENVOLUMEMESH_INCLUDE_DIR OpenVolumeMesh/Mesh/PolyhedralMesh.hh
	  PATHS /usr/local/include 
                /usr/include 
                /usr/local/OpenVolumeMesh/include
                "${CMAKE_SOURCE_DIR}/OpenVolumeMesh/src"
                "${CMAKE_SOURCE_DIR}/libs_required/OpenVolumeMesh/src"
                "${CMAKE_SOURCE_DIR}/../OpenVolumeMesh/src"
                /ACG/acgdev/gcc-4.0-x86_64/OVM/OpenVolumeMesh/installed/include
		    "C:\\Program Files\\OpenVolumeMesh\\include"
                )

IF (OPENVOLUMEMESH_INCLUDE_DIR )
   SET(OPENVOLUMEMESH_FOUND TRUE)
IF (WIN32)
	IF(EXISTS "${OPENVOLUMEMESH_INCLUDE_DIR}/../lib")
		SET(OPENVOLUMEMESH_LIBRARY_DIR "${OPENVOLUMEMESH_INCLUDE_DIR}/../lib")
	ENDIF()
ELSE (WIN32)
	IF(EXISTS "${OPENVOLUMEMESH_INCLUDE_DIR}/../lib/OpenVolumeMesh")
   		SET(OPENVOLUMEMESH_LIBRARY_DIR "${OPENVOLUMEMESH_INCLUDE_DIR}/../lib/OpenVolumeMesh")
	ENDIF()
ENDIF (WIN32)
   SET(OPENVOLUMEMESH_LIBRARY "OpenVolumeMesh")
ELSE (OPENVOLUMEMESH_INCLUDE_DIR)
   SET(OPENVOLUMEMESH_FOUND FALSE )
ENDIF (OPENVOLUMEMESH_INCLUDE_DIR )

