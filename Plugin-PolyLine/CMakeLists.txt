include (plugin)



if ( WIN32 )
  # Do not use frounding flag
else ( WIN32 )
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -frounding-math")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -frounding-math")
endif ( WIN32 )

if (EXISTS ${CMAKE_SOURCE_DIR}/libs/PhySim)
  add_definitions (-DUSE_PHYSIM)

openflipper_plugin ( OPTDEPS PhySim  
		     INSTALLDATA Icons
		    )
else()
  openflipper_plugin ( INSTALLDATA Icons )
endif ()
