# - Try to find Qwt6
# Once done this will define
#  QWT6_FOUND - System has Qwt6
#  QWT6_INCLUDE_DIRS - The Qwt6 include directories
#  QWT6_LIBRARIES - The libraries needed to use Qwt6
#  QWT6_DEFINITIONS - Compiler switches required for using Qwt6

find_path(QWT6_INCLUDE_DIR qwt.h
               PATHS ${QT_INCLUDE_DIR}
               /usr/local/qwt/include
               /usr/local/include
               /opt/local/include
               /opt/local/include/qwt
               /usr/include
               c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.1.0
               c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.2
               c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.1
               c:\\libs\\Qwt-6.1.0
               c:\\libs\\Qwt-6.0.2
               c:\\libs\\Qwt-6.0.1
               c:\\libs\\qwt-6.1.0
               c:\\libs\\qwt-6.0.2
               c:\\libs\\qwt-6.0.1
               PATH_SUFFIXES qwt qwt6 qwt-6.1.0 qwt-6.0.2 qwt-6.0.1 include qwt/include qwt6/include
               ENV PATH)

if (EXISTS "${QWT6_INCLUDE_DIR}/qwt_global.h")
  file( READ ${QWT6_INCLUDE_DIR}/qwt_global.h QWT_GLOBAL_H )
  string( REGEX MATCH "#define *QWT_VERSION *(0x06*)" QWT6_VERSION_FOUND ${QWT_GLOBAL_H})
  if (NOT QWT6_VERSION_FOUND)
    message(WARNING "Qwt is not version 6")
  endif()
else()
  message(WARNING "Could not find qwt_global.h")
endif()

if (QWT6_VERSION_FOUND)
  find_library(QWT6_LIBRARY NAMES qwt
    PATHS
    "c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.2\\lib"
    "c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.1.0\\lib"
    "c:\\libs\\Qwt-6.1.0\\lib"
    "c:\\libs\\Qwt-6.0.2\\lib"
    "c:\\libs\\qwt-6.0.1\\lib"
    /usr/local/qwt/lib
    /opt/local/lib
    /usr/local/lib
    /usr/lib
    )

  set(QWT6_LIBRARIES ${QWT6_LIBRARY} )
  set(QWT6_INCLUDE_DIRS ${QWT6_INCLUDE_DIR} )
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(QWT6  DEFAULT_MSG
                                  QWT6_LIBRARY QWT6_INCLUDE_DIR)

mark_as_advanced(QWT6_INCLUDE_DIR QWT6_LIBRARY)
