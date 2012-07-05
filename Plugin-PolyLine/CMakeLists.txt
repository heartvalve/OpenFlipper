include (plugin)



if ( WIN32 )
  # Do not use frounding flag
else ( WIN32 )
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -frounding-math")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -frounding-math")
endif ( WIN32 )

if (EXISTS ${CMAKE_SOURCE_DIR}/libs/PhySim)
  add_definitions (-DUSE_PHYSIM)

FIND_PACKAGE( Boost 1.42.0 COMPONENTS system filesystem regex )

openflipper_plugin (LICENSEMANAGER
		    DIRS 
		    DEPS Qwt5 CGAL PhySim CoMISo GMP Boost
		    OPTDEPS Taucs
		    INSTALLDATA Icons
	LIBRARIES
		${Boost_LIBRARIES}
		    )
else()
  openflipper_plugin ( LICENSEMANAGER
                       INSTALLDATA Icons )
endif ()
