# fill string with spaces
macro (of_format_string str length return)
    string (LENGTH "${str}" _str_len)
    math (EXPR _add_chr "${length} - ${_str_len}")
    set (${return} "${str}")
    while (_add_chr GREATER 0)
        set (${return} "${${return}} ")
        math (EXPR _add_chr "${_add_chr} - 1")
    endwhile ()
endmacro ()

# print message with color escape sequences if CMAKE_COLOR_MAKEFILE is set
string (ASCII 27 _escape)
function (of_color_message _str)
    if (CMAKE_COLOR_MAKEFILE AND NOT WIN32)
        message (${_str})
    else ()
        string (REGEX REPLACE "${_escape}.[0123456789;]*m" "" __str ${_str})
        message (${__str})
    endif ()
endfunction ()

# info header
function (of_print_configure_header _name)
    of_format_string ("${_name}" 40 _project)
    of_format_string ("${VERSION}" 40 _version)
    of_color_message ("\n${_escape}[40;37m************************************************************${_escape}[0m")
    of_color_message ("${_escape}[40;37m* ${_escape}[1;31mACG ${_escape}[0;40;34mBuildsystem${_escape}[0m${_escape}[40;37m                                          *${_escape}[0m")
    of_color_message ("${_escape}[40;37m*                                                          *${_escape}[0m")
    of_color_message ("${_escape}[40;37m* Package : ${_escape}[32m${_project} ${_escape}[37m      *${_escape}[0m")
    of_color_message ("${_escape}[40;37m* Version : ${_escape}[32m${_version} ${_escape}[37m      *${_escape}[0m")
    of_color_message ("${_escape}[40;37m************************************************************${_escape}[0m")
endfunction ()

# info line
function (of_print_configure_footer)
    of_color_message ("${_escape}[40;37m************************************************************${_escape}[0m\n")
endfunction ()

# print plugin statistics
function (of_print_plugin_stats)
    of_color_message ("\n${_escape}[4mPlugin configure check results:${_escape}[0m\n")
    file (
      GLOB _plugins_in
      RELATIVE "${CMAKE_SOURCE_DIR}"
      "${CMAKE_SOURCE_DIR}/Plugin-*"
    )

    
foreach (_plugin ${_plugins_in})
  string (REPLACE "Plugin-" "" _plugin_name ${_plugin})
  string (TOUPPER ${_plugin_name} _PLUGIN)
  if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${_plugin}/CMakeLists.txt AND DISABLE_QMAKE_BUILD)
    list (APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/${_plugin}")
  elseif (DISABLE_PLUGIN_${_PLUGIN})
    list (APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/${_plugin}")
  endif ()
endforeach ()

    foreach (_plugin ${_plugins_in})
        string (REPLACE "Plugin-" "" _plugin_name ${_plugin})
        string (TOUPPER ${_plugin_name} _PLUGIN)
        of_format_string (${_plugin_name} 20 _plugin_name)

        if (DISABLE_PLUGIN_${_PLUGIN})
            of_color_message ("  ${_plugin_name}: ${_escape}[1;34mDisabled${_escape}[0m")
        elseif (NOT EXISTS ${CMAKE_SOURCE_DIR}/${_plugin}/CMakeLists.txt)
            of_color_message ("  ${_plugin_name}: ${_escape}[1;34mNo CMake build system${_escape}[0m")
        else ()
            if (OPENFLIPPER_${_PLUGIN}_BUILD)
                of_color_message ("  ${_plugin_name}: ${_escape}[1;32mYes${_escape}[0m")
            else ()
                of_color_message ("  ${_plugin_name}: ${_escape}[1;31mNo${_escape}[0m (Missing dependencies :${_${_PLUGIN}_MISSING_DEPS})")
            endif ()
        endif ()
    endforeach ()
    message ("")
endfunction ()

