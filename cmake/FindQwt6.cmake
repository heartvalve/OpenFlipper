# - Try to find Qwt6
# Once done this will define
#  QWT6_FOUND - System has Qwt6
#  QWT6_INCLUDE_DIRS - The Qwt6 include directories
#  QWT6_LIBRARIES - The libraries needed to use Qwt6
#  QWT6_DEFINITIONS - Compiler switches required for using Qwt6



if ( CMAKE_GENERATOR MATCHES "^Visual Studio 11.*Win64" )
  SET(VS_SEARCH_PATH "c:/libs/vs2012/x64/")
elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 11.*" )
  SET(VS_SEARCH_PATH "c:/libs/vs2012/x32/")
elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 12.*Win64" )
  SET(VS_SEARCH_PATH "c:/libs/vs2013/x64/")
elseif ( CMAKE_GENERATOR MATCHES "^Visual Studio 12.*" )
  SET(VS_SEARCH_PATH "c:/libs/vs2013/x32/")
else ()
  SET(VS_SEARCH_PATH "")
endif()


if (QT5_FOUND)
    SET(ACG_SEARCH_PATH "/ACG/acgdev/gcc-4.9-x86_64/qwt-6.1-qt5")
else()
    SET(ACG_SEARCH_PATH "/usr/local/qwt/")
endif()


find_path(QWT6_INCLUDE_DIR qwt.h
               PATHS ${QT_INCLUDE_DIR}
               "${ACG_SEARCH_PATH}/include"
               /usr/local/qwt/include
               /usr/local/include
               /opt/local/include
               /opt/local/include/qwt
               /usr/include
               "${VS_SEARCH_PATH}/qwt-6.1.0"
               c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.1.0
               c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.2
               c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.1
               c:\\libs\\Qwt-6.1.0
               c:\\libs\\Qwt-6.0.2
               c:\\libs\\Qwt-6.0.1
               c:\\libs\\qwt-6.1.0
               c:\\libs\\qwt-6.0.2
               c:\\libs\\qwt-6.0.1
               #PATH_SUFFIXES qwt qwt6 qwt-6.1.0 qwt-6.0.2 qwt-6.0.1 include qwt/include qwt6/include #deactivated for acg dir
               ENV PATH
               )

if (EXISTS "${QWT6_INCLUDE_DIR}/qwt_global.h")
  file( READ ${QWT6_INCLUDE_DIR}/qwt_global.h QWT_GLOBAL_H )
  string( REGEX MATCH "#define *QWT_VERSION *(0x06*)" QWT6_VERSION_FOUND ${QWT_GLOBAL_H})
  if (NOT QWT6_VERSION_FOUND)
    message(WARNING "Qwt is not version 6")
  endif()
endif()

if (QWT6_VERSION_FOUND)
  if (WIN32)
    find_library(QWT6_LIBRARY_RELEASE NAMES qwt
      PATHS
      "${VS_SEARCH_PATH}/qwt-6.1.0/lib"
      "c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.2\\lib"
      "c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.1.0\\lib"
      "c:\\libs\\Qwt-6.1.0\\lib"
      "c:\\libs\\Qwt-6.0.2\\lib"
      "c:\\libs\\qwt-6.0.1\\lib"
      )
    find_library(QWT6_LIBRARY_DEBUG NAMES qwtd
      PATHS
      "${VS_SEARCH_PATH}/qwt-6.1.0/lib"
      "c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.2\\lib"
      "c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.1.0\\lib"
      "c:\\libs\\Qwt-6.1.0\\lib"
      "c:\\libs\\Qwt-6.0.2\\lib"
      "c:\\libs\\qwt-6.0.1\\lib"
  
    )

    #sets the library dirs
    if ( QWT6_LIBRARY_RELEASE )
      get_filename_component(_QWT6_LIBRARY_DIR_RELEASE ${QWT6_LIBRARY_RELEASE} PATH)
      set (QWT6_LIBRARY_DIR "${_QWT6_LIBRARY_DIR_RELEASE}" CACHE PATH "The directory where the QWT6 libraries can be found.")
    endif()

    #sets the library dirs
    if ( QWT6_LIBRARY_DEBUG )
      get_filename_component(_QWT6_LIBRARY_DIR_DEBUG ${QWT6_LIBRARY_DEBUG} PATH)
      set (QWT6_LIBRARY_DIR "${_QWT6_LIBRARY_DIR_DEBUG}" CACHE PATH "The directory where the QWT6 libraries can be found.")
    endif()                                                                                                                      

    set (
      QWT6_LIBRARY
      "optimized;${QWT6_LIBRARY_RELEASE};debug;${QWT6_LIBRARY_DEBUG}" CACHE
      STRING "QWT6 Libraries"
      )
# MACOS and LINUX
  else()
    find_library(QWT6_LIBRARY NAMES qwt-qt5
      PATHS
      "${ACG_SEARCH_PATH}/lib"
      NO_CMAKE_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      )
  
    if (NOT QWT6_LIBRARY) # if not the specific acg qwt installation was found, try system wide
      find_library(QWT6_LIBRARY NAMES qwt
        PATHS
        /usr/local/qwt/lib
        /opt/local/lib
        /usr/local/lib
        /usr/lib
        )
    endif()

    #sets the library dir 
    get_filename_component(_QWT6_LIBRARY_DIR ${QWT6_LIBRARY} PATH)
    set (QWT6_LIBRARY_DIR "${_QWT6_LIBRARY_DIR}" CACHE PATH "The directory where the QWT6 libraries can be found.")

  endif()

  set(QWT6_LIBRARIES ${QWT6_LIBRARY} )
  set(QWT6_INCLUDE_DIRS ${QWT6_INCLUDE_DIR} )
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(QWT6  DEFAULT_MSG
                                  QWT6_LIBRARY QWT6_INCLUDE_DIR)

mark_as_advanced(QWT6_INCLUDE_DIR QWT6_LIBRARY)
