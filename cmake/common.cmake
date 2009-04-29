if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    message (SEND_ERROR "Building in the source directory is not supported.")
    message (FATAL_ERROR "Please remove the created \"CMakeCache.txt\" file, the \"CMakeFiles\" directory and create a build directory and call \"${CMAKE_COMMAND} <path to the sources>\".")
endif ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")

set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
mark_as_advanced (CMAKE_CONFIGURATION_TYPES)

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: Debug, Release."
      FORCE)
endif ()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/Build)
  file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build)
endif ()

# read version from file
macro (get_version)
    file (READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" _file)
    string (
        REGEX REPLACE
        "^.*VERSION=([^\n]*).*$" "\\1"
        _version ${_file}
    )
    string (
        REGEX REPLACE
        "^.*MAJOR=([^\n]*).*$" "\\1"
        _major ${_file}
    )
    string (
        REGEX REPLACE
        "^.*MINOR=([^\n]*).*$" "\\1"
        _minor ${_file}
    )
    string (
        REGEX REPLACE
        "^.*PATCH=([^\n]*).*$" "\\1"
        _patch ${_file}
    )
    set (VERSION ${_version})
    set (VERSION_MAJOR ${_major})
    set (VERSION_MINOR ${_minor})
    set (VERSION_PATCH ${_patch})
endmacro (get_version)


include (AddFileDependencies)
include (compiler)

add_definitions (-DINCLUDE_TEMPLATES)

if (WIN32)
  set (OPENFLIPPER_APPDIR "..")
  set (OPENFLIPPER_DATADIR ".")
  set (OPENFLIPPER_PLUGINDIR "Plugins")
  set (OPENFLIPPER_LIBDIR "bin")
  set (OPENFLIPPER_BINDIR "bin")
  add_definitions(
    -D_USE_MATH_DEFINES -DNOMINMAX
    -DOPENFLIPPER_APPDIR="${OPENFLIPPER_APPDIR}"
    -DOPENFLIPPER_PLUGINDIR="${OPENFLIPPER_PLUGINDIR}"
    -DOPENFLIPPER_DATADIR="${OPENFLIPPER_DATADIR}"
  )
  if (NOT EXISTS ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_BINDIR})
    file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_BINDIR})
  endif ()
elseif (APPLE)
  set (OPENFLIPPER_APPDIR "../Resources/")
  set (MAC_OPENFLIPPER_BASEDIR "OpenFlipper.app/Contents/Resources")
  set (OPENFLIPPER_DATADIR "${MAC_OPENFLIPPER_BASEDIR}")
  set (OPENFLIPPER_PLUGINDIR "${MAC_OPENFLIPPER_BASEDIR}/Plugins")
  set (OPENFLIPPER_LIBDIR "${MAC_OPENFLIPPER_BASEDIR}/../MacOS")
  set (OPENFLIPPER_BINDIR "${MAC_OPENFLIPPER_BASEDIR}/../MacOS")
  add_definitions(
    -DOPENFLIPPER_APPDIR="../Resources"
    -DOPENFLIPPER_PLUGINDIR="Plugins"
    -DOPENFLIPPER_DATADIR="."
  )
else ()
  set (OPENFLIPPER_APPDIR "..")
  set (OPENFLIPPER_DATADIR "share/OpenFlipper")
  set (OPENFLIPPER_PLUGINDIR "lib/OpenFlipper/plugins")
  set (OPENFLIPPER_LIBDIR "lib/OpenFlipper")
  set (OPENFLIPPER_BINDIR "bin")
  add_definitions(
    -DOPENFLIPPER_APPDIR="${OPENFLIPPER_APPDIR}"
    -DOPENFLIPPER_PLUGINDIR="${OPENFLIPPER_PLUGINDIR}"
    -DOPENFLIPPER_DATADIR="${OPENFLIPPER_DATADIR}"
  )
endif ()


macro (set_target_props target)
  if (WIN32)
    set_target_properties (
      ${target} PROPERTIES
      BUILD_WITH_INSTALL_RPATH 1
      SKIP_BUILD_RPATH 0
    )
  elseif (NOT APPLE)
    set_target_properties (
      ${target} PROPERTIES
      INSTALL_RPATH "$ORIGIN/../lib/OpenFlipper"
      BUILD_WITH_INSTALL_RPATH 1
      SKIP_BUILD_RPATH 0
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_BINDIR}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_LIBDIR}"
    )
  endif ()
endmacro ()

if (NOT QT4_FOUND)
  find_package (Qt4 REQUIRED 4.5)

  set (QT_USE_QTOPENGL 1)
  set (QT_USE_QTNETWORK 1)
  set (QT_USE_QTSCRIPT 1)
  set (QT_USE_QTSQL 1)
  set (QT_USE_QTHELP 1)
  set (QT_USE_QTWEBKIT 1)
  set (QT_USE_QTUITOOLS 1)

  include (${QT_USE_FILE})
endif ()

# unsets the given variable
macro (of_unset var)
    set (${var} "" CACHE INTERNAL "")
endmacro ()

# sets the given variable
macro (of_set var value)
    set (${var} ${value} CACHE INTERNAL "")
endmacro ()

if (NOT OPENMP_NOTFOUND)
  find_package(OpenMP)
  if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    add_definitions(-DUSE_OPENMP)
  else ()
    set (OPENMP_NOTFOUND 1)
  endif ()
endif ()

macro (ftgl)
  find_package (Freetype)

  if (FREETYPE_FOUND)
    find_package (FTGL)

    if (FTGL_FOUND)
      add_definitions (-DUSE_FTGL)
      include_directories (${FTGL_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR_freetype2})
      set (FTGL_LIBS ${FREETYPE_LIBRARIES} ${FTGL_LIBRARIES})
    endif ()
  endif ()
  
endmacro ()

# append all files with extension "ext" in the "dirs" directories to "ret"
macro (append_files ret ext)
  foreach (_dir ${ARGN})
    file (GLOB _files "${_dir}/${ext}")
    list (APPEND ${ret} ${_files})
  endforeach ()
endmacro ()

macro (append_files_recursive ret ext)
  foreach (_dir ${ARGN})
    file (GLOB_RECURSE _files "${_dir}/${ext}")
    list (APPEND ${ret} ${_files})
  endforeach ()
endmacro ()

macro (drop_templates list)
  foreach (_file ${${list}})
    if (_file MATCHES "T.cc$")
      list (REMOVE_ITEM ${list} ${_file})
    endif ()
  endforeach ()
endmacro ()


macro (qt4_automoc moc_SRCS)
  qt4_get_moc_flags (_moc_INCS)

  set (_matching_FILES )
  foreach (_current_FILE ${ARGN})

     get_filename_component (_abs_FILE ${_current_FILE} ABSOLUTE)
     # if "SKIP_AUTOMOC" is set to true, we will not handle this file here.
     # here. this is required to make bouic work correctly:
     # we need to add generated .cpp files to the sources (to compile them),
     # but we cannot let automoc handle them, as the .cpp files don't exist yet when
     # cmake is run for the very first time on them -> however the .cpp files might
     # exist at a later run. at that time we need to skip them, so that we don't add two
     # different rules for the same moc file
     get_source_file_property (_skip ${_abs_FILE} SKIP_AUTOMOC)

     if ( NOT _skip AND EXISTS ${_abs_FILE} )

        file (READ ${_abs_FILE} _contents)

        get_filename_component (_abs_PATH ${_abs_FILE} PATH)

        string (REGEX MATCHALL "Q_OBJECT" _match "${_contents}")
        if (_match)
            get_filename_component (_basename ${_current_FILE} NAME_WE)
            set (_header ${_abs_FILE})
            set (_moc    ${CMAKE_CURRENT_BINARY_DIR}/moc_${_basename}.cpp)

            add_custom_command (OUTPUT ${_moc}
                COMMAND ${QT_MOC_EXECUTABLE}
                ARGS ${_moc_INCS} ${_header} -o ${_moc}
                DEPENDS ${_header}
            )

            add_file_dependencies (${_abs_FILE} ${_moc})
            set (${moc_SRCS} ${${moc_SRCS}} ${_moc})
            
        endif ()
     endif ()
  endforeach ()
endmacro ()

macro (qt4_autouic uic_SRCS)

  set (_matching_FILES )
  foreach (_current_FILE ${ARGN})

     get_filename_component (_abs_FILE ${_current_FILE} ABSOLUTE)

     if ( EXISTS ${_abs_FILE} )

        file (READ ${_abs_FILE} _contents)

        get_filename_component (_abs_PATH ${_abs_FILE} PATH)

        get_filename_component (_basename ${_current_FILE} NAME_WE)
        string (REGEX REPLACE "Ui$" "" _cbasename ${_basename})
        set (_outfile ${CMAKE_CURRENT_BINARY_DIR}/ui_${_basename}.hh)
        set (_header ${_basename}.hh)
        set (_source ${_abs_PATH}/${_cbasename}.cc)
        
        add_custom_command (OUTPUT ${_outfile}
            COMMAND ${QT_UIC_EXECUTABLE}
            ARGS -o ${_outfile} ${_abs_FILE}
            DEPENDS ${_abs_FILE})

        add_file_dependencies (${_source} ${_outfile})
        set (${uic_SRCS} ${${uic_SRCS}} ${_outfile})
            
     endif ()
  endforeach ()
endmacro ()

function (add_plugins)
    file (
        GLOB _plugins_in
        RELATIVE "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/Plugin-*/CMakeLists.txt"
    )

    foreach (_plugin ${_plugins_in})
        get_filename_component (_plugin_dir ${_plugin} PATH)
        add_subdirectory (${CMAKE_SOURCE_DIR}/${_plugin_dir})
    endforeach ()
endfunction ()

macro (get_files_in_dir ret dir)
  file (GLOB_RECURSE __files RELATIVE "${dir}" "${dir}/*")
  foreach (_file ${__files})
    if (NOT _file MATCHES ".*svn.*")
      list (APPEND ${ret} "${_file}")
    endif ()
  endforeach ()
endmacro ()

function (copy_after_build target src dst)
  of_unset (_files)
  get_files_in_dir (_files ${src})
  foreach (_file ${_files})
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${src}/${_file}" "${dst}/${_file}"
    )
  endforeach ()
endfunction ()

function (install_dir src dst)
  of_unset (_files)
  get_files_in_dir (_files ${src})
  foreach (_file ${_files})
    get_filename_component (_file_PATH ${_file} PATH)
    install(FILES "${src}/${_file}"
      DESTINATION "${dst}/${_file_PATH}"
    )
  endforeach ()
endfunction ()
- 1,288 ----
# prevent build in source directory
if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    message (SEND_ERROR "Building in the source directory is not supported.")
    message (FATAL_ERROR "Please remove the created \"CMakeCache.txt\" file, the \"CMakeFiles\" directory and create a build directory and call \"${CMAKE_COMMAND} <path to the sources>\".")
endif ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")

# allow only Debug and Release builds
set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
mark_as_advanced (CMAKE_CONFIGURATION_TYPES)

# set Debus as default build target
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: Debug, Release."
      FORCE)
endif ()

# create our output directroy
if (NOT EXISTS ${CMAKE_BINARY_DIR}/Build)
  file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build)
endif ()

include (AddFileDependencies)
include (compiler)

# define INCLUDE_TEMPLATES for everything we build
add_definitions (-DINCLUDE_TEMPLATES)

# set directory structures for the different platforms
if (WIN32)
  set (OPENFLIPPER_APPDIR "..")
  set (OPENFLIPPER_DATADIR ".")
  set (OPENFLIPPER_PLUGINDIR "Plugins")
  set (OPENFLIPPER_LIBDIR "bin")
  set (OPENFLIPPER_BINDIR "bin")
  add_definitions(
    -D_USE_MATH_DEFINES -DNOMINMAX
    -DOPENFLIPPER_APPDIR="${OPENFLIPPER_APPDIR}"
    -DOPENFLIPPER_PLUGINDIR="${OPENFLIPPER_PLUGINDIR}"
    -DOPENFLIPPER_DATADIR="${OPENFLIPPER_DATADIR}"
  )
  if (NOT EXISTS ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_BINDIR})
    file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_BINDIR})
  endif ()
elseif (APPLE)
  set (OPENFLIPPER_APPDIR "../Resources/")
  set (MAC_OPENFLIPPER_BASEDIR "OpenFlipper.app/Contents/Resources")
  set (OPENFLIPPER_DATADIR "${MAC_OPENFLIPPER_BASEDIR}")
  set (OPENFLIPPER_PLUGINDIR "${MAC_OPENFLIPPER_BASEDIR}/Plugins")
  set (OPENFLIPPER_LIBDIR "${MAC_OPENFLIPPER_BASEDIR}/../MacOS")
  set (OPENFLIPPER_BINDIR "${MAC_OPENFLIPPER_BASEDIR}/../MacOS")
  add_definitions(
    -DOPENFLIPPER_APPDIR="../Resources"
    -DOPENFLIPPER_PLUGINDIR="Plugins"
    -DOPENFLIPPER_DATADIR="."
  )
else ()
  set (OPENFLIPPER_APPDIR "..")
  set (OPENFLIPPER_DATADIR "share/OpenFlipper")
  set (OPENFLIPPER_PLUGINDIR "lib/OpenFlipper/plugins")
  set (OPENFLIPPER_LIBDIR "lib/OpenFlipper")
  set (OPENFLIPPER_BINDIR "bin")
  add_definitions(
    -DOPENFLIPPER_APPDIR="${OPENFLIPPER_APPDIR}"
    -DOPENFLIPPER_PLUGINDIR="${OPENFLIPPER_PLUGINDIR}"
    -DOPENFLIPPER_DATADIR="${OPENFLIPPER_DATADIR}"
  )
endif ()

# set basic build properties for the different platforms
macro (set_target_props target)
  if (WIN32)
    set_target_properties (
      ${target} PROPERTIES
      BUILD_WITH_INSTALL_RPATH 1
      SKIP_BUILD_RPATH 0
    )
  elseif (NOT APPLE)
    set_target_properties (
      ${target} PROPERTIES
      INSTALL_RPATH "$ORIGIN/../lib/OpenFlipper"
      BUILD_WITH_INSTALL_RPATH 1
      SKIP_BUILD_RPATH 0
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_BINDIR}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_LIBDIR}"
    )
  endif ()
endmacro ()

# look for selected qt dependencies
if (NOT QT4_FOUND)
  find_package (Qt4 REQUIRED 4.5)

  set (QT_USE_QTOPENGL 1)
  set (QT_USE_QTNETWORK 1)
  set (QT_USE_QTSCRIPT 1)
  set (QT_USE_QTSQL 1)
  set (QT_USE_QTHELP 1)
  set (QT_USE_QTWEBKIT 1)
  set (QT_USE_QTUITOOLS 1)

  include (${QT_USE_FILE})
endif ()

# unsets the given variable
macro (of_unset var)
    set (${var} "" CACHE INTERNAL "")
endmacro ()

# sets the given variable
macro (of_set var value)
    set (${var} ${value} CACHE INTERNAL "")
endmacro ()

# test for OpenMP
if (NOT OPENMP_NOTFOUND)
  find_package(OpenMP)
  if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    add_definitions(-DUSE_OPENMP)
  else ()
    set (OPENMP_NOTFOUND 1)
  endif ()
endif ()

# test for FTGL
macro (ftgl)
  find_package (Freetype)

  if (FREETYPE_FOUND)
    find_package (FTGL)

    if (FTGL_FOUND)
      add_definitions (-DUSE_FTGL)
      include_directories (${FTGL_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR_freetype2})
      set (FTGL_LIBS ${FREETYPE_LIBRARIES} ${FTGL_LIBRARIES})
    endif ()
  endif ()
  
endmacro ()

# append all files with extension "ext" in the "dirs" directories to "ret"
macro (append_files ret ext)
  foreach (_dir ${ARGN})
    file (GLOB _files "${_dir}/${ext}")
    list (APPEND ${ret} ${_files})
  endforeach ()
endmacro ()

# append all files with extension "ext" in the "dirs" directories and its subdirectories to "ret"
macro (append_files_recursive ret ext)
  foreach (_dir ${ARGN})
    file (GLOB_RECURSE _files "${_dir}/${ext}")
    list (APPEND ${ret} ${_files})
  endforeach ()
endmacro ()

# drop all "*T.cc" files from list
macro (drop_templates list)
  foreach (_file ${${list}})
    if (_file MATCHES "T.cc$")
      list (REMOVE_ITEM ${list} ${_file})
    endif ()
  endforeach ()
endmacro ()

# generate moc targets for sources in list
macro (qt4_automoc moc_SRCS)
  qt4_get_moc_flags (_moc_INCS)

  set (_matching_FILES )
  foreach (_current_FILE ${ARGN})

     get_filename_component (_abs_FILE ${_current_FILE} ABSOLUTE)
     # if "SKIP_AUTOMOC" is set to true, we will not handle this file here.
     # here. this is required to make bouic work correctly:
     # we need to add generated .cpp files to the sources (to compile them),
     # but we cannot let automoc handle them, as the .cpp files don't exist yet when
     # cmake is run for the very first time on them -> however the .cpp files might
     # exist at a later run. at that time we need to skip them, so that we don't add two
     # different rules for the same moc file
     get_source_file_property (_skip ${_abs_FILE} SKIP_AUTOMOC)

     if ( NOT _skip AND EXISTS ${_abs_FILE} )

        file (READ ${_abs_FILE} _contents)

        get_filename_component (_abs_PATH ${_abs_FILE} PATH)

        string (REGEX MATCHALL "Q_OBJECT" _match "${_contents}")
        if (_match)
            get_filename_component (_basename ${_current_FILE} NAME_WE)
            set (_header ${_abs_FILE})
            set (_moc    ${CMAKE_CURRENT_BINARY_DIR}/moc_${_basename}.cpp)

            add_custom_command (OUTPUT ${_moc}
                COMMAND ${QT_MOC_EXECUTABLE}
                ARGS ${_moc_INCS} ${_header} -o ${_moc}
                DEPENDS ${_header}
            )

            add_file_dependencies (${_abs_FILE} ${_moc})
            set (${moc_SRCS} ${${moc_SRCS}} ${_moc})
            
        endif ()
     endif ()
  endforeach ()
endmacro ()

# generate uic targets for sources in list
macro (qt4_autouic uic_SRCS)

  set (_matching_FILES )
  foreach (_current_FILE ${ARGN})

     get_filename_component (_abs_FILE ${_current_FILE} ABSOLUTE)

     if ( EXISTS ${_abs_FILE} )

        file (READ ${_abs_FILE} _contents)

        get_filename_component (_abs_PATH ${_abs_FILE} PATH)

        get_filename_component (_basename ${_current_FILE} NAME_WE)
        string (REGEX REPLACE "Ui$" "" _cbasename ${_basename})
        set (_outfile ${CMAKE_CURRENT_BINARY_DIR}/ui_${_basename}.hh)
        set (_header ${_basename}.hh)
        set (_source ${_abs_PATH}/${_cbasename}.cc)
        
        add_custom_command (OUTPUT ${_outfile}
            COMMAND ${QT_UIC_EXECUTABLE}
            ARGS -o ${_outfile} ${_abs_FILE}
            DEPENDS ${_abs_FILE})

        add_file_dependencies (${_source} ${_outfile})
        set (${uic_SRCS} ${${uic_SRCS}} ${_outfile})
            
     endif ()
  endforeach ()
endmacro ()

# search all plugins and add them to build
function (add_plugins)
    file (
        GLOB _plugins_in
        RELATIVE "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/Plugin-*/CMakeLists.txt"
    )

    foreach (_plugin ${_plugins_in})
        get_filename_component (_plugin_dir ${_plugin} PATH)
        add_subdirectory (${CMAKE_SOURCE_DIR}/${_plugin_dir})
    endforeach ()
endfunction ()

# get all files in directory, but ignore svn
macro (get_files_in_dir ret dir)
  file (GLOB_RECURSE __files RELATIVE "${dir}" "${dir}/*")
  foreach (_file ${__files})
    if (NOT _file MATCHES ".*svn.*")
      list (APPEND ${ret} "${_file}")
    endif ()
  endforeach ()
endmacro ()

# copy the whole directory without svn files
function (copy_after_build target src dst)
  of_unset (_files)
  get_files_in_dir (_files ${src})
  foreach (_file ${_files})
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${src}/${_file}" "${dst}/${_file}"
    )
  endforeach ()
endfunction ()

# install the whole directory without svn files
function (install_dir src dst)
  of_unset (_files)
  get_files_in_dir (_files ${src})
  foreach (_file ${_files})
    get_filename_component (_file_PATH ${_file} PATH)
    install(FILES "${src}/${_file}"
      DESTINATION "${dst}/${_file_PATH}"
    )
  endforeach ()
endfunction ()
