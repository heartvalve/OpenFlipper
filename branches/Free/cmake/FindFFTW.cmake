# - Try to find FFTW
# Once done this will define
#  
#  FFTW_FOUND        - system has FFTW
#  FFTW_INCLUDE_DIR  - the FFTW include directory
#  FFTW_LIBRARY_DIR  - where the libraries are
#  FFTW_LIBRARY      - Link these to use FFTW
#   

IF (FFTW_INCLUDE_DIR)
  # Already in cache, be silent
  SET(FFTW_FIND_QUIETLY TRUE)
ENDIF (FFTW_INCLUDE_DIR)

if( WIN32 )
  # FIND_PATH( FFTW_INCLUDE_DIR gl/glew.h gl/wglew.h
  #            PATHS "c:/glew/include" "c:/libs/glew/include" ${COMPILER_PATH}/PlatformSDK/Include )
  # SET( FFTW_NAMES glew32 )
  # FIND_LIBRARY( FFTW_LIBRARY
  #               NAMES ${FFTW_NAMES}
  #               PATHS "c:/glew/lib"  "c:/libs/glew/lib" ${COMPILER_PATH}/PlatformSDK/Lib )
else( WIN32 )
   FIND_PATH( FFTW_INCLUDE_DIR fftw3.h
              PATHS /usr/local/include /usr/include )

   IF( APPLE)
     SET( FFTW_LIBRARIES fftw3 fftw3_threads )
   else( APPLE )
     SET( FFTW_LIBRARIES fftw3 fftw3l fftw3f fftw3_threads fftw3l_threads fftw3f_threads )
   endif(APPLE)

   FIND_LIBRARY( FFTW_LIBRARY_LOCATION
                 NAMES ${FFTW_LIBRARIES}
                 PATHS /usr/lib /usr/local/lib /opt/local/lib )
endif( WIN32 )

GET_FILENAME_COMPONENT( FFTW_LIBRARY_DIR ${FFTW_LIBRARY_LOCATION} PATH )

IF (FFTW_INCLUDE_DIR AND FFTW_LIBRARY_DIR)
   SET(FFTW_FOUND TRUE)
ELSE (FFTW_INCLUDE_DIR AND FFTW_LIBRARY_DIR)
   SET( FFTW_FOUND FALSE )
   SET( FFTW_LIBRARY_DIR )
ENDIF (FFTW_INCLUDE_DIR AND FFTW_LIBRARY_DIR)

