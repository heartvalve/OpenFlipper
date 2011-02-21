if (OPENNI_INCLUDE_DIR)
  # in cache already
  set(OPENNI_FOUND TRUE)
else (OPENNI_INCLUDE_DIR)


find_path(OPENNI_INCLUDE_DIR NAMES XnOpenNI.h
	  PATHS /usr/include/ni "C:/Program Files/OpenNI/Include"
	  ${OPENNI_INCLUDE_PATH}
          )

find_library( OPENNI_LIBRARY 
              OpenNI
              PATHS /usr/lib "C:/Program Files/OpenNI/Lib" )

if (OPENNI_INCLUDE_DIR)
  # in cache already
  set(OPENNI_FOUND TRUE)
endif(OPENNI_INCLUDE_DIR)

endif(OPENNI_INCLUDE_DIR)

