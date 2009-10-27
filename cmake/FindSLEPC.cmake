# - Try to find SLEPC
# Once done this will define
#
#  SLEPC_FOUND        - system has SLEPSc
#  SLEPC_INCLUDE_DIR  - the SLEPSc include directories
#  SLEPC_LIBRARY      - Link this to use SLEPSc

FIND_PACKAGE(PETSC)

find_path (SLEPC_INCLUDE_DIR slepc.h
  HINTS ENV SLEPC_INCLUDE_DIR
  PATHS /usr/lib/slepcdir/3.0.0/include  /opt/local/lib/petsc/include "C:\\libs\\slepc-3.0.0-p4\\include"
  DOC "SLEPC Include Directory")

IF ( WIN32 )
find_library( SLEPC_LIBRARY libslepc.lib
              PATHS "C:\\libs\\slepc-3.0.0-p4\\cygwin-c-opt\\lib"
            )
ELSE( WIN32 )
  find_library( SLEPC_LIBRARY slepc
                PATHS /usr/lib
                      /opt/local/lib/petsc/lib 
              )
ENDIF( WIN32 )

IF (SLEPC_INCLUDE_DIR AND SLEPC_LIBRARY)
  SET(SLEPC_FOUND TRUE)
ELSE ()
  SET(SLEPC_FOUND FALSE)
ENDIF ()




