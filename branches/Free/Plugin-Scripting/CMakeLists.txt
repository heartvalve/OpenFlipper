if (QT_QTSCRIPTTOOLS_FOUND)
  if ( NOT DEFINED OPENFLIPPER_ENABLE_SCRIPT_DEBUGGER )
    set( OPENFLIPPER_ENABLE_SCRIPT_DEBUGGER false CACHE BOOL "Enable or disable qt script debugger in OpenFlippers scripting system" )
  endif() 

  if ( OPENFLIPPER_ENABLE_SCRIPT_DEBUGGER )
    add_definitions(-DENABLE_SCRIPT_DEBUGGER)
  endif()
endif()

include (plugin)
openflipper_plugin ( TRANSLATION_LANGUAGES de_DE 
                     INSTALLDATA Icons )
