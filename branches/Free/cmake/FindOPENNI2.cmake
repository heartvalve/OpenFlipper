# Try to find OPENMESH
# Once done this will define
#  
# OPENNI2_FOUND           - system has OPENNI2
# OPENNI2_INCLUDE_DIRS    - the OPENNI2 include directories
# OPENNI2_LIBRARIES       - Link these to use OPENNI2
#
#=======================================

if (OPENNI2_INCLUDE_DIR AND OPENNI2_LIBRARY)
  # in cache already
  set(OPENNI2_FOUND TRUE)
else (OPENNI2_INCLUDE_DIR AND OPENNI2_LIBRARY)


  find_path(OPENNI2_INCLUDE_DIR NAMES OpenNI.h
	  PATHS 
	  /usr/local/OpenNI-Linux-x64-2.2/Include
	  "C:/Program Files/OpenNI/Include" 
	  $ENV{OPENNI2_INCLUDE}
          )

  find_library( OPENNI2_LIBRARY 
              OpenNI2
              PATHS 
			  /usr/local/OpenNI-Linux-x64-2.2/Redist 
			  "C:/Program Files/OpenNI/Lib" 
			  $ENV{OPENNI2_REDIST} )

  if (OPENNI2_INCLUDE_DIR AND OPENNI2_LIBRARY) 

    set(OPENNI2_FOUND TRUE)
    set(OPENNI2_INCLUDE_DIRS ${OPENNI2_INCLUDE_DIR} )
	set(OPENNI2_LIBRARIES ${OPENNI2_LIBRARY} )
	mark_as_advanced(OPENNI2_INCLUDE_DIR OPENNI2_LIBRARY)
  
  endif(OPENNI2_INCLUDE_DIR AND OPENNI2_LIBRARY)

endif(OPENNI2_INCLUDE_DIR AND OPENNI2_LIBRARY)

