set (ACG_PROJECT_MACOS_BUNDLE "1")

macro (acg_modify_project_dirs)
  # set directory structures for the different platforms
  if (WIN32)
    add_definitions(
      -D_USE_MATH_DEFINES -DNOMINMAX
      -DOPENFLIPPER_APPDIR="."
      -DOPENFLIPPER_PLUGINDIR="${ACG_PROJECT_PLUGINDIR}"
      -DOPENFLIPPER_DATADIR="${ACG_PROJECT_DATADIR}"
    )
    if (NOT EXISTS ${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_BINDIR})
      file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_BINDIR})
    endif ()
  elseif (APPLE)
    set (MAC_OPENFLIPPER_BASEDIR "OpenFlipper.app/Contents/Resources")
    set (ACG_PROJECT_DATADIR "${MAC_OPENFLIPPER_BASEDIR}")
    set (ACG_PROJECT_PLUGINDIR "${MAC_OPENFLIPPER_BASEDIR}/Plugins")
    set (ACG_PROJECT_LIBDIR "${MAC_OPENFLIPPER_BASEDIR}/../MacOS")
    set (ACG_PROJECT_BINDIR "${MAC_OPENFLIPPER_BASEDIR}/../MacOS")
    add_definitions(
      -DOPENFLIPPER_APPDIR="../Resources"
      -DOPENFLIPPER_PLUGINDIR="Plugins"
      -DOPENFLIPPER_DATADIR="."
    )
  else ()
    add_definitions(
      -DOPENFLIPPER_APPDIR=".."
      -DOPENFLIPPER_PLUGINDIR="${ACG_PROJECT_PLUGINDIR}"
      -DOPENFLIPPER_DATADIR="${ACG_PROJECT_DATADIR}"
    )
  endif ()
endmacro ()

# search all plugins and add them to build
function (of_add_plugins)
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

# print plugin statistics
function (of_print_plugin_stats)

  acg_color_message ("\n${_escape}[4mPlugin configure check results:${_escape}[0m\n")
  file (
    GLOB _plugins_in
    RELATIVE "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/Plugin-*"
  )

  list (SORT _plugins_in)

  foreach (_plugin ${_plugins_in})
    string (REPLACE "Plugin-" "" _plugin_name ${_plugin})
    string (TOUPPER ${_plugin_name} _PLUGIN)
    if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${_plugin}/CMakeLists.txt AND DISABLE_QMAKE_BUILD)
      list (APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/${_plugin}")
    elseif (DISABLE_PLUGIN_${_PLUGIN})
      list (APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/${_plugin}")
    endif ()
  endforeach ()


  # Sort plugins into lists depending on configure status:
  set (PLUGINS_OK "")
  set (PLUGINS_DISABLED "")
  set (PLUGINS_DEPENDENCIES "")
  set (PLUGINS_NO_CMAKE "")

  foreach (_plugin ${_plugins_in})
     string (REPLACE "Plugin-" "" _plugin_name ${_plugin})
     string (TOUPPER ${_plugin_name} _PLUGIN)
     acg_format_string (${_plugin_name} 25 _plugin_name)

     if (DISABLE_PLUGIN_${_PLUGIN})
          list( APPEND PLUGINS_DISABLED ${_plugin} )
      elseif (NOT EXISTS ${CMAKE_SOURCE_DIR}/${_plugin}/CMakeLists.txt)
           list( APPEND PLUGINS_NO_CMAKE ${_plugin} )
      else ()
          if (OPENFLIPPER_${_PLUGIN}_BUILD)
              list( APPEND PLUGINS_OK ${_plugin} )
          else ()
              list( APPEND PLUGINS_DEPENDENCIES ${_plugin} )
          endif ()
      endif ()
  endforeach ()

  message ("")

  # Print all plugins, which have no cmake build system
  acg_color_message ("\n${_escape}[4mPlugins without cmake build system:${_escape}[0m\n")
  foreach (_plugin ${PLUGINS_NO_CMAKE})
    string (REPLACE "Plugin-" "" _plugin_name ${_plugin})
    string (TOUPPER ${_plugin_name} _PLUGIN)
    acg_format_string (${_plugin_name} 25 _plugin_name)
 
    acg_color_message ("  ${_plugin_name}: ${_escape}[1;34mNo CMake build system${_escape}[0m")
  endforeach ()

  message ("")

  # Print all plugins, which are ok
  acg_color_message ("\n${_escape}[4mPlugins configured successfully:${_escape}[0m\n")
  foreach (_plugin ${PLUGINS_OK})
    string (REPLACE "Plugin-" "" _plugin_name ${_plugin})
    string (TOUPPER ${_plugin_name} _PLUGIN)
    acg_format_string (${_plugin_name} 25 _plugin_name)
 
    acg_color_message ("  ${_plugin_name}: ${_escape}[1;32mYes${_escape}[0m")
  endforeach ()

  message ("")


  # Print all plugins, which are disabled
  acg_color_message ("\n${_escape}[4mPlugins disabled:${_escape}[0m\n")
  foreach (_plugin ${PLUGINS_DISABLED})
    string (REPLACE "Plugin-" "" _plugin_name ${_plugin})
    string (TOUPPER ${_plugin_name} _PLUGIN)
    acg_format_string (${_plugin_name} 25 _plugin_name)
                                                                                     
    acg_color_message ("  ${_plugin_name}: ${_escape}[1;34mDisabled${_escape}[0m")
  endforeach ()

  message ("")



  # Print all plugins, which have missing dependencies
  acg_color_message ("\n${_escape}[4mPlugins with missing dependencies:${_escape}[0m\n")
  foreach (_plugin ${PLUGINS_DEPENDENCIES})
    string (REPLACE "Plugin-" "" _plugin_name ${_plugin})
    string (TOUPPER ${_plugin_name} _PLUGIN)
    acg_format_string (${_plugin_name} 25 _plugin_name)
                                                                                     
    acg_color_message ("  ${_plugin_name}: ${_escape}[1;31mNo${_escape}[0m (Missing dependencies :${_${_PLUGIN}_MISSING_DEPS})")
  endforeach ()
  
  message ("")
 

  if ( NOT WIN32 )
    message("${_escape}[4mCurrent Build Type:${_escape}[0m ${_escape}[1;34m ${CMAKE_BUILD_TYPE} ${_escape}[0m")
  endif()
  message ("")


endfunction ()
