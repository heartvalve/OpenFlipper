# - Try to find ASSIMP
# Once done this will define
#  
#  ASSIMP_FOUND        - system has ASSIMP
#  ASSIMP_INCLUDE_DIR  - the ASSIMP include directory
#  ASSIMP_LIBRARY      - Link these to use ASSIMP
#  ASSIMP_LIBRARY_DIR  - Directory where the library is found 
#   

IF (ASSIMP_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(ASSIMP_FIND_QUIETLY TRUE)
ENDIF (ASSIMP_INCLUDE_DIRS)

 # Find the headers
 FIND_PATH( ASSIMP_INCLUDE_DIR assimp/scene.h
            PATHS /usr/include 
                  "C:/Program Files/Assimp/include" )

if( WIN32 )

 if ( CMAKE_GENERATOR MATCHES ".*Win64" )
   SET( DIRSUFFIX "x64" )
 else ()
   SET( DIRSUFFIX "x86" )
 endif()

  if ( CMAKE_GENERATOR MATCHES "^Visual Studio 10.*" )
    SET(VS_SEARCH_PATH "c:/libs/vs2010/x32/")
  elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 11.*Win64" )
    SET(VS_SEARCH_PATH "c:/libs/vs2012/x64/")
  elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 11.*" )
    SET(VS_SEARCH_PATH "c:/libs/vs2012/x32/")
  elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 12.*Win64" )
    SET(VS_SEARCH_PATH "c:/libs/vs2013/x64/")
  elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 12.*" )
    SET(VS_SEARCH_PATH "c:/libs/vs2013/x32/")
  endif()


 FIND_LIBRARY( ASSIMP_LIBRARY
               NAMES assimp
	       PATH_SUFFIXES ${DIRSUFFIX}
               PATHS "C:/libs/assimp/lib"
                     "${VS_SEARCH_PATH}assimp" 
	             "C:/Program Files/Assimp/lib"
               )  


 GET_FILENAME_COMPONENT( ASSIMP_LIBRARY_DIR ${ASSIMP_LIBRARY} PATH ) 

 SET( ASSIMP_LIBRARY_DIR ${ASSIMP_LIBRARY_DIR}/../../bin/${DIRSUFFIX}/ CACHE PATH "Path to the DLLs")
 
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

