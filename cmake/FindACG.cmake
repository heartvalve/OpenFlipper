# - Try to find ACG
# Once done this will define
#  
# ACG_FOUND           - system has ACG
# ACG_INCLUDE_DIR - theACG include directory
# ACG_LIBRARY         - Link these to use ACG
# ACG_LIBRARY_DIR  - Library DIR of ACG
#   

IF (ACG_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(ACG_FIND_QUIETLY TRUE)
ENDIF (ACG_INCLUDE_DIRS)

FIND_PATH(ACG_INCLUDE_DIR QtWidgets/QtBaseViewer.hh
	  PATHS /usr/local/include 
                /usr/include 
                "${CMAKE_SOURCE_DIR}/ACG"
                "${CMAKE_SOURCE_DIR}/../ACG"
                )

IF (ACG_INCLUDE_DIR )
   SET(ACG_FOUND TRUE)
   # TODO: Set correct library path!!
   #SET(ACG_LIBRARY_DIR "${ACG_INCLUDE_DIR}/../")
   #SET(ACG_LIBRARY "OpenMeshCore OpenMeshTools")
ELSE (ACG_INCLUDE_DIR)
   SET(ACG_FOUND FALSE )
ENDIF (ACG_INCLUDE_DIR )

