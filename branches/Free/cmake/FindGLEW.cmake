# - Try to find GLEW
# Once done this will define
#  
#  GLEW_FOUND        - system has GLEW
#  GLEW_INCLUDE_DIR  - the GLEW include directory
#  GLEW_LIBRARY_DIR  - where the libraries are
#  GLEW_LIBRARY      - Link these to use GLEW
#   

IF (GLEW_INCLUDE_DIR)
  # Already in cache, be silent
  SET(GLEW_FIND_QUIETLY TRUE)
ENDIF (GLEW_INCLUDE_DIR)

if( WIN32 )

  if ( CMAKE_GENERATOR MATCHES "^Visual Studio 11.*Win64" )
    SET(VS_SEARCH_PATH "c:/libs/vs2012/x64/")
  elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 11.*" )
    SET(VS_SEARCH_PATH "c:/libs/vs2012/x32/")
  elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 12.*Win64" )
    SET(VS_SEARCH_PATH "c:/libs/vs2013/x64/")
  elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 12.*" )
    SET(VS_SEARCH_PATH "c:/libs/vs2013/x32/")
  endif()

   if( MSVC80 )
       set( COMPILER_PATH "C:/Program\ Files/Microsoft\ Visual\ Studio\ 8/VC" )
   endif( MSVC80 )
   if( MSVC71 )
       set( COMPILER_PATH "C:/Program\ Files/Microsoft\ Visual\ Studio\ .NET\ 2003/Vc7" )
   endif( MSVC71 )

   FIND_PATH( GLEW_INCLUDE_DIR gl/glew.h gl/wglew.h
              PATHS "c:/glew/include" 
                    "c:/libs/glew/include" 
                    "c:/libs/glew-1.6.0/include" 
                    ${COMPILER_PATH}/PlatformSDK/Include 
                    "${VS_SEARCH_PATH}glew-1.10.0/include")

   SET( GLEW_NAMES glew32 )
   FIND_LIBRARY( GLEW_LIBRARY
                 NAMES ${GLEW_NAMES}
                 PATHS "c:/glew/lib"  
                       "c:/libs/glew/lib" 
                       "c:/libs/glew-1.6.0/lib" 
                       ${COMPILER_PATH}/PlatformSDK/Lib 
                       "${VS_SEARCH_PATH}glew-1.10.0/lib" )

else( WIN32 )
   FIND_PATH( GLEW_INCLUDE_DIR GL/glew.h GL/wglew.h
              PATHS /usr/local/include /usr/include )
   SET( GLEW_NAMES glew GLEW )
   FIND_LIBRARY( GLEW_LIBRARY
                 NAMES ${GLEW_NAMES}
                 PATHS /usr/lib /usr/local/lib )
endif( WIN32 )

GET_FILENAME_COMPONENT( GLEW_LIBRARY_DIR ${GLEW_LIBRARY} PATH )

IF (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
   SET(GLEW_FOUND TRUE)
ELSE (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
   SET( GLEW_FOUND FALSE )
   SET( GLEW_LIBRARY_DIR )
ENDIF (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)

