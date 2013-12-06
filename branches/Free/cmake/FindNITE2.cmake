# Try to find OPENMESH
# Once done this will define
#  
# NITE2_FOUND           - system has NITE2
# NITE2_INCLUDE_DIRS    - the NITE2 include directories
# NITE2_LIBRARIES       - Link these to use NITE2
#
#=======================================

if (NITE2_INCLUDE_DIR AND NITE2_LIBRARY)
  # in cache already
  set(NITE2_FOUND TRUE)
else (NITE2_INCLUDE_DIR AND NITE2_LIBRARY)


  find_path(NITE2_INCLUDE_DIR NAMES NiTE.h
	  PATHS 
	  /usr/local/OpenNI-Linux-x64-2.2/NiTE-Linux-x64-2.2/Include
	  "C:/Program Files/PrimeSense/NiTE2/Include" 
#	  $ENV{NITE2_INCLUDE}
          )

  find_library( NITE2_LIBRARY 
              NITE2
              PATHS 
			  /usr/local/OpenNI-Linux-x64-2.2/NiTE-Linux-x64-2.2/Redist 
			  "C:/Program Files/PrimeSense/NiTE2/Lib" 
			  $ENV{NITE2_REDIST} )

  if (NITE2_INCLUDE_DIR AND NITE2_LIBRARY) 

    set(NITE2_FOUND TRUE)
    set(NITE2_INCLUDE_DIRS ${NITE2_INCLUDE_DIR} )
	set(NITE2_LIBRARIES ${NITE2_LIBRARY} )
	mark_as_advanced(NITE2_INCLUDE_DIR NITE2_LIBRARY)
  
  endif(NITE2_INCLUDE_DIR AND NITE2_LIBRARY)

endif(NITE2_INCLUDE_DIR AND NITE2_LIBRARY)

