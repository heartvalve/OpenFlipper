if ( NOT CGAL_GENERATOR_SPECIFIC_SETTINGS_FILE_INCLUDED )
  set( CGAL_GENERATOR_SPECIFIC_SETTINGS_FILE_INCLUDED 1 )
 
  if ( MSVC )
    set(CGAL_AUTO_LINK_ENABLED TRUE)
  endif()

  if (MSVC12)
    set(CGAL_TOOLSET "vc120")
  elseif ( MSVC11 )  
    set(CGAL_TOOLSET "vc110")	
  elseif ( MSVC10 )  
    set(CGAL_TOOLSET "vc100")	
  elseif ( MSVC90 )  
    set(CGAL_TOOLSET "vc90")
  elseif ( MSVC80 )  
    set(CGAL_TOOLSET "vc80")
  elseif ( MSVC71 )
    set(CGAL_TOOLSET "vc71")
  endif()

  
  # From james Bigler, in the cmake users list.
  IF (APPLE)
    exec_program(uname ARGS -v  OUTPUT_VARIABLE DARWIN_VERSION)
    string(REGEX MATCH "[0-9]+" DARWIN_VERSION ${DARWIN_VERSION})
    if (DARWIN_VERSION GREATER 8)
      set(CGAL_APPLE_LEOPARD 1)
    endif()
  endif()
  
  if ( NOT "${CMAKE_CFG_INTDIR}" STREQUAL "." )
    set(HAS_CFG_INTDIR TRUE CACHE INTERNAL "Generator uses intermediate configuration directory" )
  endif()

endif()
