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

#SET(ACG_LIBRARY_DIR "" CACHE INTERNAL "Path to ACG Library")

FIND_PATH(ACG_INCLUDE_DIR ACG/QtWidgets/QtBaseViewer.hh
	  PATHS /usr/local/include 
                /usr/include 
                "${CMAKE_SOURCE_DIR}/"
                "${CMAKE_SOURCE_DIR}/../"
                /ACG/acgdev/gcc-4.3-x86_64/OM2/ACG-2.0/include
                )

IF (ACG_INCLUDE_DIR )

   SET(ACG_FOUND TRUE)

   # TODO: Set correct library path!!
   SET(ACG_LIBRARY_DIR "${ACG_INCLUDE_DIR}/../lib/" CACHE PATH "Path to ACG Library")
   SET(ACG_LIBRARY "ACG")
   IF(WIN32)
     add_definitions (-DACGDLL -DUSEACG )
   ENDIF(WIN32)
ELSE (ACG_INCLUDE_DIR)
   SET(ACG_FOUND FALSE )
ENDIF (ACG_INCLUDE_DIR )

