# - Try to find SLEPC
# Once done this will define
#
#  SLEPC_FOUND        - system has SLEPSc
#  SLEPC_INCLUDE_DIR  - the SLEPSc include directories
#  SLEPC_LIBRARY      - Link this to use SLEPSc

FIND_PACKAGE(PETSC)

find_path (SLEPC_INCLUDE_DIR slepc.h
  HINTS ENV SLEPC_INCLUDE_DIR
  PATHS /usr/lib/slepcdir/3.0.0/include
  DOC "SLEPC Include Directory")


find_library( SLEPC_LIBRARY slepc
              PATHS /usr/lib
            )

IF (SLEPC_INCLUDE_DIR AND SLEPC_LIBRARY)
  SET(SLEPC_FOUND TRUE)
ELSE ()
  SET(SLEPC_FOUND FALSE)
ENDIF ()




