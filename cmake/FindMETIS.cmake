# - Try to find METIS
# Once done this will define
#  METIS_FOUND - System has Metis
#  METIS_INCLUDE_DIRS - The Metis include directories
#  METIS_LIBRARY_DIRS - The library directories needed to use Metis
#  METIS_LIBRARIES    - The libraries needed to use Metis

if (METIS_INCLUDE_DIR)
  # in cache already
  SET(METIS_FIND_QUIETLY TRUE)
endif (METIS_INCLUDE_DIR)

if ( WIN32 ) 

 if ( CMAKE_GENERATOR MATCHES ".*Win64" )
   SET( DIRSUFFIX "lib64" )
 else ()
   SET( DIRSUFFIX "lib" )
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

find_path(METIS_INCLUDE_DIR NAMES metis.h
     PATHS "$ENV{IPOPT_HOME}/ThirdParty/Metis/metis-4.0/Lib/"        
	       "${VS_SEARCH_PATH}suitesparse-metis-for-windows-1.2.2-install/include/"
           
   )
   
find_library( METIS_LIBRARY 
              metis metis
              PATHS "${VS_SEARCH_PATH}suitesparse-metis-for-windows-1.2.2-install/${DIRSUFFIX}" )
					
else(WIN32)

find_path(METIS_INCLUDE_DIR NAMES metis.h
     PATHS "$ENV{IPOPT_HOME}/ThirdParty/Metis/metis-4.0/Lib/"
           "/usr/include/"
           "/usr/include/metis"
           "/opt/local/include"
           "/opt/local/include/metis"
           
           
   )
   
find_library( METIS_LIBRARY 
              metis coinmetis
              PATHS "$ENV{IPOPT_HOME}/lib/"
                    "/usr/lib"
                    "/opt/local/lib" )
					
endif()					

set(METIS_INCLUDE_DIRS "${METIS_INCLUDE_DIR}" )
set(METIS_LIBRARIES "${METIS_LIBRARY}" )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCPLEX_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(METIS  DEFAULT_MSG
                                  METIS_LIBRARY METIS_INCLUDE_DIR)

mark_as_advanced(METIS_INCLUDE_DIR METIS_LIBRARY )
