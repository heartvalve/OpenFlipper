if (NITE_INCLUDE_DIR)
  # in cache already
  set(NITE_FOUND TRUE)
else (NITE_INCLUDE_DIR)


find_path(NITE_INCLUDE_DIR NAMES XnVNite.h
	  PATHS /usr/include "C:/Program Files/Prime Sense/NITE/Include"
	  ${NITE_INCLUDE_PATH}
      s)

find_library( NITE_LIBRARY 
              XnVNITE
              PATHS /usr/lib "C:/Program Files/Prime Sense/NITE/Lib" )

if (NITE_INCLUDE_DIR)
  # in cache already
  set(NITE_FOUND TRUE)
endif(NITE_INCLUDE_DIR)

endif(NITE_INCLUDE_DIR)

