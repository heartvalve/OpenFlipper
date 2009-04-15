# This module provides the following macro:
#
# openflipper_plugin ( [DIRS dir1 dir2 ...]
#                      [DEPS dep1 dep2 ...]
#                      [OPTDEPS dep1 dep2 ...]
#                      [LDFLAGSADD flag1 flag2 ...]
#                      [CFLAGSADD flag1 flag2 ...]
#                      [LIBRARIES lib1 lib2 ...]
#                      [LIBDIRS dir1 dir2 ...]
#                      [INCDIRS dir1 dir2 ...])
#
# DIRS       = additional directories with source files
# DEPS       = required dependencies for find_package macro
# OPTDEPS    = optional dependencies for find_package macro
# LDFLAGSADD = flags added to the link command
# CFLAGSADD  = flags added to the compile command
# LIBRARIES  = libraries added to link command
# LIBDIRS    = additional link directories
# INCDIRS    = additional include directories

include (common)

# get plugin name from directory name
macro (_get_plugin_name var)
  string (REGEX MATCH "Plugin-.+[/\\]?$" _dir ${CMAKE_CURRENT_SOURCE_DIR})
  string (REPLACE "Plugin-" "" ${var} ${_dir})
endmacro ()

# parse plugin macro parameter
macro (_get_plugin_parameters _prefix)
    set (_current_var _foo)
    set (_supported_var DIRS DEPS OPTDEPS LDFLAGSADD CFLAGSADD LIBRARIES LIBDIRS INCDIRS)
    foreach (_val ${_supported_var})
        set (${_prefix}_${_val})
    endforeach ()
    foreach (_val ${ARGN})
        set (_found FALSE)
        foreach (_find ${_supported_var})
            if ("${_find}" STREQUAL "${_val}")
                set (_found TRUE)
            endif ()
        endforeach ()
        
        if (_found)
            set (_current_var ${_prefix}_${_val})
        else ()
            list (APPEND ${_current_var} ${_val})
        endif ()
    endforeach ()
endmacro ()

# check dependencies
macro (_check_plugin_deps _prefix)
    set (${_prefix}_HAS_DEPS TRUE)
    foreach (_val ${ARGN})
        string (TOUPPER ${_val} _VAL)

        find_package(${_val})

        if (${_val}_FOUND OR ${_VAL}_FOUND)
          foreach (_name ${_val} ${_VAL})
            if (DEFINED ${_name}_INCLUDE_DIRS)
              list (APPEND ${_prefix}_DEPS_INCDIRS "${${_name}_INCLUDE_DIRS}")
            endif ()
            if (DEFINED ${_name}_LIBRARY_DIRS)
              list (APPEND ${_prefix}_DEPS_LIBDIRS "${${_name}_LIBRARY_DIRS}")
            endif ()
            if (DEFINED ${_name}_LIBRARIES)
              list (APPEND ${_prefix}_DEPS_LIBRARIES "${${_name}_LIBRARIES}")
            endif ()
            if (DEFINED ${_name}_LINKER_FLAGS)
              list (APPEND ${_prefix}_DEPS_LINKER_FLAGS "${${_name}_LINKER_FLAGS}")
            endif ()
          endforeach ()
        else ()
            set (${_prefix}_HAS_DEPS FALSE)
            of_set (_${_prefix}_MISSING_DEPS "${_${_prefix}_MISSING_DEPS} ${_val}")
        endif ()
    endforeach ()
endmacro ()

# main function
function (_build_openflipper_plugin plugin)
  find_package (OpenGL)
  find_package (GLUT)
  find_package (GLEW)

  string (TOUPPER ${plugin} _PLUGIN)

  _get_plugin_parameters (${_PLUGIN} ${ARGN})

  

  # check dependencies
  of_unset (_${_PLUGIN}_MISSING_DEPS)
  set (${_PLUGIN}_HAS_DEPS)
  _check_plugin_deps (${_PLUGIN} ${${_PLUGIN}_OPTDEPS})

  of_unset (_${_PLUGIN}_MISSING_DEPS)
  set (${_PLUGIN}_HAS_DEPS)
  _check_plugin_deps (${_PLUGIN} ${${_PLUGIN}_DEPS})

  if (${_PLUGIN}_HAS_DEPS)
    include_directories (
      .
      ${CMAKE_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${${_PLUGIN}_DEPS_INCDIRS}
      ${${_PLUGIN}_INCDIRS}
      ${OPENGL_INCLUDE_DIR}
      ${GLEW_INCLUDE_DIR}
      ${GLUT_INCLUDE_DIR}
    )

    link_directories (
      ${${_PLUGIN}_DEPS_LIBDIRS}
      ${${_PLUGIN}_LIBDIRS}
    )

    set (directories 
      . 
      ${${_PLUGIN}_DIRS}
    )

    append_files (headers "*.hh" ${directories})
    append_files (sources "*.cc" ${directories})
    append_files (ui "*.ui" ${directories})

    drop_templates (sources)

    qt4_autouic (uic_targets ${ui})
    qt4_automoc (moc_targets ${headers})

    add_library (Plugin-${plugin} MODULE ${uic_targets} ${sources} ${headers} ${moc_targets})


    set_target_properties (
      Plugin-${plugin} PROPERTIES
      COMPILE_FLAGS "${${_PLUGIN}_CFLAGSADD}"
      LINK_FLAGS "${${_PLUGIN}_LDFLAGSADD} ${${_PLUGIN}_DEPS_LINKER_FLAGS}"
    )
    
    if (WIN32)
      find_package (GLUT)
      add_definitions (-DACGDLL -DUSEACG -DPLUGINLIBDLL -DUSEPLUGINLIBDLL)
      target_link_libraries (Plugin-${plugin}      
	OpenMeshCore
        OpenMeshTools
        ACG
        PluginLib
        ${GLUT_LIBRARIES}
      )
      if (NOT EXISTS ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_PLUGINDIR})
        file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_PLUGINDIR})
      endif ()
      add_custom_command (TARGET Plugin-${plugin} POST_BUILD
                          COMMAND ${CMAKE_COMMAND} -E
                          copy_if_different
                            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Plugin-${plugin}.dll
                            ${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_PLUGINDIR}
                          )
    else ()
      set_target_properties (
        Plugin-${plugin} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/${OPENFLIPPER_PLUGINDIR}"
      )
    endif ()
    
    target_link_libraries (Plugin-${plugin}
      ${${_PLUGIN}_DEPS_LIBRARIES}
      ${${_PLUGIN}_LIBRARIES}
    )
    
    install (
      TARGETS Plugin-${plugin}
      DESTINATION ${OPENFLIPPER_PLUGINDIR}
    )

  else ()
    message (STATUS "[WARNING] One or more dependencies for plugin ${plugin} not found. Skipping plugin.")
    message (STATUS "Missing dependencies :${_${_PLUGIN}_MISSING_DEPS}")
  endif ()
endfunction ()

macro (openflipper_plugin)
  _get_plugin_name (_plugin)

  string (TOUPPER ${_plugin} _PLUGIN)

  option (
    DISABLE_PLUGIN_${_PLUGIN}
    "Disable building of plugin \"${_plugin}\""
        OFF
  )

  if (NOT DISABLE_PLUGIN_${_PLUGIN})
    _build_openflipper_plugin (${_plugin} ${ARGN})
  endif ()
endmacro ()
