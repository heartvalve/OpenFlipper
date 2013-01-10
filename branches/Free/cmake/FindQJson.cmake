# - Try to find QJSON
# Once done this will define
#  QJSON_FOUND - System has Cplex
#  QJSON_INCLUDE_DIRS - The Cplex include directories
#  QJSON_LIBRARIES - The libraries needed to use Cplex

if (QJSON_INCLUDE_DIR)
  # in cache already
  set(QJSON_FOUND TRUE)
  set(QJSON_INCLUDE_DIRS "${QJSON_INCLUDE_DIR};${QJSON_CONCERT_INCLUDE_DIR}" )
  set(QJSON_LIBRARIES "${QJSON_LIBRARY};${QJSON_ILO_LIBRARY};${QJSON_CONCERT_LIBRARY}" )
else (QJSON_INCLUDE_DIR)

find_path(QJSON_INCLUDE_DIR 
          NAMES QJson/Serializer QJson/Parser
          PATHS
          "${CMAKE_SOURCE_DIR}/libs_required/qjson/include"
          "/usr/local/include"
          "/usr/include"
          )
          
if (QJSON_INCLUDE_DIR STREQUAL "${CMAKE_SOURCE_DIR}/libs_required/qjson/include")
    set(QJSON_LIBRARY "qjson" CACHE FILEPATH "Path to QJson library")
else()
find_library( QJSON_LIBRARY 
              qjson
              PATHS
              "/usr/local/lib"
              "/usr/lib"
              )
endif()

set(QJSON_INCLUDE_DIRS "${QJSON_INCLUDE_DIR}" )
set(QJSON_LIBRARIES "${QJSON_LIBRARY}" )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBQJSON_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(QJSON DEFAULT_MSG
                                  QJSON_LIBRARY QJSON_INCLUDE_DIR)

mark_as_advanced(QJSON_INCLUDE_DIR QJSON_LIBRARY)

endif(QJSON_INCLUDE_DIR)
