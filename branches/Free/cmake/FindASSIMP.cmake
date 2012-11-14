# - Try to find ASSIMP
# Once done this will define
#  
#  ASSIMP_FOUND        - system has ASSIMP
#  ASSIMP_INCLUDE_DIR  - the ASSIMP include directory
#  ASSIMP_LIBRARY      - Link these to use ASSIMP
#   

IF (ASSIMP_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(ASSIMP_FIND_QUIETLY TRUE)
ENDIF (ASSIMP_INCLUDE_DIRS)

 # Find the headers
 FIND_PATH( ASSIMP_INCLUDE_DIR assimp/scene.h
            PATHS /usr/include  )

if( WIN32 )
 FIND_LIBRARY( ASSIMP_LIBRARY
               NAMES libassimp.lib
               PATHS "C:/libs/assimp/lib" )  

 GET_FILENAME_COMPONENT( ASSIMP_LIBRARY_DIR ${ASSIMP_LIBRARY} PATH ) 
else (WIN32)

 FIND_LIBRARY( ASSIMP_LIBRARY
               NAMES assimp
               PATHS /usr/lib /usr/local/lib )
endif( WIN32)


IF (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)
  SET(ASSIMP_FOUND TRUE)
ELSE (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)
  SET( ASSIMP_FOUND FALSE )
ENDIF (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)

