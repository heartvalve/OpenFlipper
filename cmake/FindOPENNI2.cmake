if (OPENNI2_INCLUDE_DIR)
  # in cache already
  set(OPENNI2_FOUND TRUE)
else (OPENNI2_INCLUDE_DIR)


find_path(OPENNI2_INCLUDE_DIR NAMES OpenNI.h
	  PATHS /usr/local/OpenNI-Linux-x64-2.2/Include
	  $ENV{OPENNI2_INCLUDE}
          )

find_library( OPENNI2_LIBRARY 
              OpenNI2
              PATHS /usr/local/OpenNI-Linux-x64-2.2/Redist "C:/Program Files/OpenNI/Lib" $ENV{OPENNI2_REDIST} )

if (OPENNI2_INCLUDE_DIR)
  # in cache already
  set(OPENNI2_FOUND TRUE)
endif(OPENNI2_INCLUDE_DIR)

endif(OPENNI2_INCLUDE_DIR)

