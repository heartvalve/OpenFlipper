if (NITE2_INCLUDE_DIR)
  # in cache already
  set(NITE2_FOUND TRUE)
else (NITE2_INCLUDE_DIR)


find_path(NITE2_INCLUDE_DIR NAMES NiTE.h
	  PATHS /usr/local/OpenNI-Linux-x64-2.2/NiTE-Linux-x64-2.2/Include
# 	  $ENV{NITE2_INCLUDE}
          )

find_library( NITE2_LIBRARY 
              NiTE2 
              PATHS /usr/local/OpenNI-Linux-x64-2.2/NiTE-Linux-x64-2.2/Redist "C:/Program Files/OpenNI/Lib" $ENV{NITE2_REDIST64} )

if (NITE2_INCLUDE_DIR)
  # in cache already
  set(NITE2_FOUND TRUE)
endif(NITE2_INCLUDE_DIR)

endif(NITE2_INCLUDE_DIR)

