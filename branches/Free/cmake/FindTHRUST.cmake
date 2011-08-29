# Finder for thrust library
# thrust is template only, so only includes are searched
# 
# THRUST_INCLUDE_DIR Toplevel include dir containing thrust, includes have to be defined like <thrust/header.h>


IF (THRUST_INCLUDE_DIR)
  # Already in cache, be silent
  SET(THRUST_FIND_QUIETLY TRUE)
ENDIF (THRUST_INCLUDE_DIR)


find_path( THRUST_INCLUDE_DIR
    HINTS /usr/include /usr/local/include
    NAMES thrust/version.h
    DOC "Lib Thrust Include dir"
)

# ensure that we only get one
if( THRUST_INCLUDE_DIR )
    list( REMOVE_DUPLICATES THRUST_INCLUDE_DIR )
endif( THRUST_INCLUDE_DIR )

# Check for required components
if ( THRUST_INCLUDE_DIR )

    # Parse version file to get the thrust version numbers

    file( STRINGS ${THRUST_INCLUDE_DIR}/thrust/version.h
        version
        REGEX "#define THRUST_VERSION[ \t]+([0-9x]+)"
    )

    string( REGEX REPLACE "#define THRUST_VERSION[ \t]+" "" version ${version} )

    string( REGEX MATCH "^[0-9]" major ${version} )
    string( REGEX REPLACE "^${major}00" "" version ${version} )
    string( REGEX MATCH "^[0-9]" minor ${version} )
    string( REGEX REPLACE "^${minor}0" "" version ${version} )
    set( THRUST_VERSION "${major}.${minor}.${version}")


    SET( THRUST_FOUND TRUE )

    include( FindPackageHandleStandardArgs )
    find_package_handle_standard_args( Thrust
        REQUIRED_VARS
            THRUST_INCLUDE_DIR
        VERSION_VAR
            THRUST_VERSION
    )
 
endif()
