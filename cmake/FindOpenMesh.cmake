# - Try to findOPENMESH
# Once done this will define
#  
# OPENMESH_FOUND           - system has OPENMESH
# OPENMESH_INCLUDE_DIR - theOPENMESH include directory
# OPENMESH_LIBRARY         - Link these to use OPENMESH
# OPENMESH_LIBRARY_DIR  - Library DIR of OPENMESH
#   

IF (OPENMESH_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(OPENMESH_FIND_QUIETLY TRUE)
ENDIF (OPENMESH_INCLUDE_DIRS)

FIND_PATH(OPENMESH_INCLUDE_DIR OpenMesh/Core/Mesh/PolyMeshT.hh
	  PATHS /usr/local/include 
                /usr/include 
                "${CMAKE_SOURCE_DIR}/OpenMesh/src"
                "${CMAKE_SOURCE_DIR}/../OpenMesh/src"
                /ACG/acgdev/gcc-4.0-x86_64/OM2/OpenMesh-2.0/installed/include
		    "C:\\Program Files\\OpenMesh 2.0\\include"
                )

IF (OPENMESH_INCLUDE_DIR )
   SET(OPENMESH_FOUND TRUE)
   SET(OPENMESH_LIBRARY_DIR "${OPENMESH_INCLUDE_DIR}/../lib/OpenMesh")
   SET(OPENMESH_LIBRARY "OpenMeshCore;OpenMeshTools")
ELSE (OPENMESH_INCLUDE_DIR)
   SET(OPENMESH_FOUND FALSE )
ENDIF (OPENMESH_INCLUDE_DIR )

