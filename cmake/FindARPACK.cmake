# - Try to find ARPACK
# Once done this will define
#
#  ARPACK_FOUND        - system has ARPACK
#  ARPACK_INCLUDE_DIR  - the ARPACK include directories
#  ARPACK_LIBRARY      - Link this to use ARPACK


find_path (ARPACK_INCLUDE_DIR NAMES arpack++/argeig.h
  HINTS ENV ARPACK_INCLUDE_DIR
  PATHS /usr/include/arpack++ "C:\\libs\\arpack++\\include"
  DOC "ARPACK Include Directory")

IF ( WIN32 )
find_library( ARPACK_LIBRARY arpack.lib
              PATHS "C:\\libs\\arpack++\\lib"
            )
ELSE( WIN32 )
  find_library( ARPACK_LIBRARY arpack arpack++
                PATHS /usr/lib 
              )
ENDIF( WIN32 )

IF (ARPACK_INCLUDE_DIR AND ARPACK_LIBRARY)
  SET(ARPACK_FOUND TRUE)
ELSE ()
  SET(ARPACK_FOUND FALSE)
ENDIF ()




