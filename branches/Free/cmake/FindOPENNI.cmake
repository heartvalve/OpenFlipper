if (OPENNI_INCLUDE_DIR)
  # in cache already
  set(OPENNI_FOUND TRUE)
message(status "check1")
else (OPENNI_INCLUDE_DIR)

message(status "check")

find_path(OPENNI_INCLUDE_DIR NAMES XnOpenNI.h
	  PATHS /usr/include/ni
	  ${OPENNI_INCLUDE_PATH}
          )

find_library( OPENNI_LIBRARY 
              OpenNI
              PATHS /usr/lib)

if (OPENNI_INCLUDE_DIR)
  # in cache already
  set(OPENNI_FOUND TRUE)
endif(OPENNI_INCLUDE_DIR)

endif(OPENNI_INCLUDE_DIR)

