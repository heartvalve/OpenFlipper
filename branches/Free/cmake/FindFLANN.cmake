# - Try to find FLANN
# Once done this will define
#  
# FLANN_FOUND           - system has FLANN
# FLANN_INCLUDE_DIR     - theUTIL include directory
# FLANN_LIBRARY         - Link these to use UTIL
# FLANN_LIBRARY_DIR     - Library DIR of UTIL
#   

IF (FLANN_INCLUDE_DIR)
 # Already in cache, be silent
 SET(FLANN_FIND_QUIETLY TRUE)
ENDIF (FLANN_INCLUDE_DIR)
                
FIND_PATH(FLANN_INCLUDE_DIR flann/flann.hpp
	  PATHS "~/opt/flann-1.7.1-src/build_rel/include"
                /opt/local/include )                

if( FLANN_INCLUDE_DIR )
   set( FLANN_FOUND TRUE )

    IF(APPLE)

       set( FLANN_LIBRARY_DIR "/opt/local/lib" )
       set( FLANN_LIBRARY optimized flann_cpp debug flann_cpp )

    ELSEIF(WIN32)

      if ( CMAKE_GENERATOR MATCHES "^Visual Studio 11.*Win64" )
        SET(VS_SEARCH_PATH "c:/libs/vs2012/x64/")
      elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 11.*" )
        SET(VS_SEARCH_PATH "c:/libs/vs2012/x32/")
      elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 12.*Win64" )
        SET(VS_SEARCH_PATH "c:/libs/vs2013/x64/")
      elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 12.*" )
        SET(VS_SEARCH_PATH "c:/libs/vs2013/x32/")
      endif()


      FIND_LIBRARY( FLANN_LIBRARY flann_cpp_s
              PATHS  "${VS_SEARCH_PATH}"
              PATH_SUFFIXES flann-1.8.4/lib
              )

    ELSE()

       set( FLANN_LIBRARY_DIR "~/opt/flann-1.7.1-src/build_rel/lib64" )
       set( FLANN_LIBRARY optimized flann_cpp debug flann_cpp )

    ENDIF()


ELSE (FLANN_INCLUDE_DIR)
   SET(FLANN_FOUND FALSE )
ENDIF (FLANN_INCLUDE_DIR )

