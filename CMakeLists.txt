cmake_minimum_required (VERSION 2.8.9)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()


IF ( APPLE )
	IF(CMAKE_BUILD_TYPE MATCHES Debug)
	    message(WARNING "WARNING: Mac Debug mode might mix debug and release libraries (Webkit, ...), which causes strange errors. If you use ports to provide qt, you might want to disable Qt debug libraries to make sure only release qt libraries are linked to OpenFlippers debug build.")
	ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
endif()


project (OpenFlipper)

# add our macro directory to cmake search path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# include our cmake files
include (ACGCommon)

acg_get_version (OpenFlipper)

# ========================================================================
# Branding Flags
# ========================================================================

if ( EXISTS ${CMAKE_SOURCE_DIR}/branding/branding.cmake )
	include(${CMAKE_SOURCE_DIR}/branding/branding.cmake)
endif()

set( OPENFLIPPER_PRODUCT_STRING "OpenFlipper" CACHE String "Product string used e.g. to determine the config file path. Should only contain alphanumeric characters (no spaces)." )
set( OPENFLIPPER_PRODUCT_NAME "OpenFlipper" CACHE String "Product name used in all visible places (e.g. about box)." )
set( BRANDING_HEADER false CACHE String "Indicate there is a branding.h inside the branding folder.")
add_definitions(-DPRODUCT_STRING=${OPENFLIPPER_PRODUCT_STRING})
add_definitions(-DPRODUCT_NAME=${OPENFLIPPER_PRODUCT_NAME})

if (BRANDING_HEADER)
add_definitions(-DBRANDING_HEADER)
endif ()

include (package)
include (ACGOutput)

# try to use QT5 if possible otherwise stick to QT4
set (FORCE_QT4 ON CACHE BOOL "Use Qt4 even if Qt5 was found")

if (NOT FORCE_QT4)
  acg_qt5 ()
else()
  set (QT5_FOUND false)
endif()


if (NOT QT5_FOUND)
  message(STATUS "Using QT4 for OpenFlipper")
  acg_qt4 (4.7.0)
else ()
  message(STATUS "Using QT5 for OpenFlipper")
endif ()

acg_openmp ()
#acg_ftgl ()

# check for OpenGL, GLEW and GLUT as our required dependencies
find_package (OpenGL)
if (NOT OPENGL_FOUND)
  message (FATAL_ERROR "OpengGL not found!")
endif ()

find_package (GLEW)
if (NOT GLEW_FOUND)
  message (FATAL_ERROR "GLEW not found!")
endif ()

find_package (GLUT)
if (NOT GLUT_FOUND)
  message (FATAL_ERROR "GLUT not found!")
endif ()

# using the internal openmesh delivered by the svn repo (default) or use some external installation
set (USE_INTERNAL_OPENMESH ON CACHE BOOL "Use and build internal version of OpenMesh found in libs_required")

# OpenMesh is a dependent project (in libs_required) so no finder is needed.
if (NOT USE_INTERNAL_OPENMESH)
  find_package (OpenMesh REQUIRED)
endif()

find_package (OpenVolumeMesh)
if (NOT OPENVOLUMEMESH_FOUND)
    message (FATAL_ERROR "OpenVolumeMesh not found!")
endif ()


# ========================================================================
# Configure the executables path for tests
# ========================================================================
if (APPLE)
  SET( OPENFLIPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/Build/OpenFlipper.app/Contents/MacOS/${OPENFLIPPER_PRODUCT_STRING}" )
elseif( WIN32 )
  SET( OPENFLIPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_PRODUCT_STRING}.exe" )
else()
  SET( OPENFLIPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/Build/bin/${OPENFLIPPER_PRODUCT_STRING}" )
endif()
SET(OPENFLIPPER_TEST_FILES    ${CMAKE_BINARY_DIR}/tests/globalTestFiles )
SET(OPENFLIPPER_TEST_RESULTS  ${CMAKE_BINARY_DIR}/tests/testResults )
SET(OPENFLIPPER_TEST_BINARIES ${CMAKE_BINARY_DIR}/tests/testBinaries )

enable_testing()

# ========================================================================
# Build all additional libraries
# ========================================================================

# search all libs in the libs directory
# (required and optional libraries
file (
  GLOB _local_libs_in
  RELATIVE "${CMAKE_SOURCE_DIR}"
           "${CMAKE_SOURCE_DIR}/libs_required/*/CMakeLists.txt"
           "${CMAKE_SOURCE_DIR}/libs/*/CMakeLists.txt"
)

set( REMAINING_LOCAL_LIBS "" )

# Special Comiso case:
# The library has to be configured first
foreach (_localLib ${_local_libs_in})
  if ( "${_localLib}" MATCHES .*CoMISo.* )
    get_filename_component (_local_lib_dir ${_localLib} PATH)
    if ( NOT DEFINED LIB_COMISO_BUILD )
      set( LIB_COMISO_BUILD true CACHE BOOL "Enable or disable build of COMISO Library" )
    endif()

    if ( LIB_COMISO_BUILD )
      add_subdirectory (${CMAKE_SOURCE_DIR}/${_local_lib_dir})
    endif()
  else()
    if ( NOT "${_localLib}" MATCHES OpenMesh OR USE_INTERNAL_OPENMESH)
      LIST(APPEND REMAINING_LOCAL_LIBS ${_localLib})
    endif()
  endif()
endforeach ()


# add all found libs to the build
foreach (_localLib ${REMAINING_LOCAL_LIBS})

  get_filename_component (_local_lib_dir ${_localLib} PATH)

  # If the library is in the required folder, we have to build it anyway, so
  # detect it here
  SET(REQUIRED FALSE)
  if ( _localLib MATCHES ".*libs_required.*" )
    SET(REQUIRED TRUE )
  endif()

  # Extract upper case library dir name for the human readable entries in the cmake config
  string(TOUPPER ${_local_lib_dir} _local_lib_dir_upper)
  string(REGEX REPLACE "LIBS[/\\]" "" _local_lib_dir_upper ${_local_lib_dir_upper} )

  # Only generate setting if the lib is not required (otherwise, we will always build it)
  if ( NOT DEFINED LIB_${_local_lib_dir_upper}_BUILD AND NOT REQUIRED)
      set( LIB_${_local_lib_dir_upper}_BUILD true CACHE BOOL "Enable or disable build of ${_local_lib_dir_upper} Library" )
  endif()

  # Build if this library is required or if it is enabled
  if ( LIB_${_local_lib_dir_upper}_BUILD OR REQUIRED)
    add_subdirectory (${CMAKE_SOURCE_DIR}/${_local_lib_dir})
  endif()
endforeach ()

# ========================================================================
# Core Library Components (Except the Core application)
# ========================================================================
add_subdirectory (ACG)
add_subdirectory (OpenFlipper/PluginLib)

# ========================================================================
# Tests
# ========================================================================
add_subdirectory (tests)

# ========================================================================
# Documentation generators
# ========================================================================

# Create dirs to make doxygen happy
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc )
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help )

# Add the documentation targets
add_subdirectory (OpenFlipper/Documentation/DeveloperTarget)
add_subdirectory (OpenFlipper/Documentation/DeveloperQtHelpTarget)
add_subdirectory (OpenFlipper/Documentation/UserTarget)
add_subdirectory (OpenFlipper/Documentation/UserQtHelpTarget)

# ========================================================================
# PolyLine extension special case
# ========================================================================

if ( EXISTS ${CMAKE_SOURCE_DIR}/libs/MeshIntegration )
  add_definitions( -DEXTENDED_POLY_LINE )
endif()

# ========================================================================
# License Manager
# ========================================================================
if ( NOT DEFINED OPENFLIPPER_ENABLE_LICENSE_MANAGER )
  set( OPENFLIPPER_ENABLE_LICENSE_MANAGER true CACHE BOOL "Enable or disable license management in all plugins" )
endif()

if ( NOT DEFINED OPENFLIPPER_ENABLE_LICENSE_KEY_GENERATOR )
  set( OPENFLIPPER_ENABLE_LICENSE_KEY_GENERATOR true CACHE BOOL "Enable or disable building the license key generators in all plugins" )
endif()

# ========================================================================
# Apple Mac Store Flags
# ========================================================================
if ( APPLE AND NOT DEFINED OPENFLIPPER_BUILD_APP_STORE_COMPLIANT )
  set( OPENFLIPPER_BUILD_APP_STORE_COMPLIANT true CACHE BOOL "Enable app store compliant build (No internal update, different paths,...)" )
endif()

# ========================================================================
# Plugins
# ========================================================================

# start with an empty plugin list
acg_set (OPENFLIPPER_PLUGINS "")

# add plugins
of_add_plugins ()

# ========================================================================
# Build core as we now know all dependencies
# ========================================================================

add_subdirectory (OpenFlipper/CoreApp)


# ========================================================================
# Bundle generation
# ========================================================================

if (WIN32 )

  # Plugins collected a list of required libdirs for getting
  # their DLLS on windows:
  get_property( WINDOWS_COPY_LIBDIRS GLOBAL PROPERTY WINDOWS_LIBRARY_DIR_LIST )

  if ( WINDOWS_COPY_LIBDIRS )
    list(REMOVE_DUPLICATES WINDOWS_COPY_LIBDIRS)
  endif()

  
  # workaround for VS2018 where the OpenMP vcomp90.dll seems to be missing from the path
  if ( CMAKE_GENERATOR MATCHES "^Visual Studio 9.*" )
    INCLUDE(InstallRequiredSystemLibraries)
    set ( OMP_PATH "${MSVC90_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC90.OPENMP/;${MSVC90_REDIST_DIR}/Debug_NonRedist/${CMAKE_MSVC_ARCH}/Microsoft.VC90.DebugOpenMP/" )
  endif()

  # prepare bundle generation cmake file and add a build target for it
  configure_file ("${CMAKE_SOURCE_DIR}/OpenFlipper/cmake/fixbundle.cmake.win.in"
     "${CMAKE_BINARY_DIR}/fixbundle.win.cmake" @ONLY IMMEDIATE)

  add_custom_target (fixbundle ALL
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/fixbundle.win.cmake" )

  # let bundle generation depend on all targets
  add_dependencies (fixbundle ${OPENFLIPPER_PRODUCT_STRING} PluginLib ${OPENFLIPPER_PLUGINS})

endif(WIN32)

# ========================================================================
# Custom command to create a bundle and a package on mac
# ========================================================================

if ( APPLE )

   # Configuration option about fixbundle
   if ( APPLE AND NOT DEFINED OPENFLIPPER_CALL_FIX_BUNDLE )
     set( OPENFLIPPER_CALL_FIX_BUNDLE false CACHE BOOL "After building OpenFlipper, a bundle creation step is executed (which copies all dependencies into the bundle) to create a valid MacOs Bundle. This step is only necessary if creating a package." )
   endif()

  # prepare bundle generation cmake file and add a build target for it
  configure_file ("${CMAKE_SOURCE_DIR}/OpenFlipper/cmake/fixbundle.cmake.in"
     "${CMAKE_BINARY_DIR}/fixbundle.cmake" @ONLY IMMEDIATE)

  # Fixbundle is only necessary, when we wan't a shippable package.
  # Otherwise the original rpaths are used
  if ( OPENFLIPPER_CALL_FIX_BUNDLE )
    # Call fixbundle every time
    add_custom_target (fixbundle ALL
      COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/fixbundle.cmake"
    )
  else()
    # Call fixbundle only as a dependency
    add_custom_target (fixbundle
      COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/fixbundle.cmake"
    )
  endif()

  # let bundle generation depend on all targets
  add_dependencies (fixbundle ${OPENFLIPPER_PRODUCT_STRING} PluginLib ${OPENFLIPPER_PLUGINS})

  # install bundle
  install (DIRECTORY ${CMAKE_BINARY_DIR}/Build/OpenFlipper.app DESTINATION .
           USE_SOURCE_PERMISSIONS)

  # Extra target for building an OpenFlipper package on mac
  # Call fixbundle first
  # afterwards we call make package as an external command, as we can't
  # add a dependency on build in targets of cake
  add_custom_target (OpenFlipper_package
    DEPENDS fixbundle
    COMMAND make package
  )

endif()

# ========================================================================
# Output log information
# ========================================================================

# display results
acg_print_configure_header (OPENFLIPPER "OpenFlipper")
of_print_plugin_stats ()
acg_print_configure_footer ()
