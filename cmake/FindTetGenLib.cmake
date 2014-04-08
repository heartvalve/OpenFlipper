# - Try to find TetGenLib
# Once done this will define
#  
#  TetGenLib_FOUND        - system has TetGenLib
#  TetGenLib_INCLUDE_DIR  - the TetGenLib include directory
#  TetGenLib_LIBRARY_DIR  - where the libraries are
#  TetGenLib_LIBRARY      - Link these to use TetGenLib
#   

IF (TetGenLib_INCLUDE_DIR)
  # Already in cache, be silent
  SET(TetGenLib_FIND_QUIETLY TRUE)
ENDIF (TetGenLib_INCLUDE_DIR)

set(TetGenLib_TMP_PATHS /home/zimmer/projects/libs/)

FIND_PATH( TetGenLib_INCLUDE_DIR tetgen.h
  HINTS ${TetGenLib_TMP_PATHS}/TetGen/include/)

if( TetGenLib_INCLUDE_DIR )
  
  if(NOT TetGenLib_FIND_QUIETLY)
    message( Found TetGenLib_INCLUDE_DIR: ${TetGenLib_INCLUDE_DIR})
  endif()

  find_library(
    TetGenLib_LIBRARY
    NAMES tetgen tet
    PATHS ${TetGenLib_TMP_PATHS}/TetGen/lib/)
  
  if( TetGenLib_LIBRARY )

    if(NOT TetGenLib_FIND_QUIETLY)
      message( Found TetGenLib_LIBRARY: ${TetGenLib_LIBRARY})
    endif()

    set(TetGenLib_LIBRARY_DIR "")
    get_filename_component(TetGenLib_LIBRARY_DIR ${TetGenLib_LIBRARY} PATH)
    # Set uncached variables as per standard.
    set(TetGenLib_FOUND ON)
    set(TetGenLib_INCLUDE_DIR ${TetGenLib_INCLUDE_DIR})
    set(TetGenLib_LIBRARY ${TetGenLib_LIBRARY})
  endif(TetGenLib_LIBRARY)
endif(TetGenLib_INCLUDE_DIR)
#message( FOUnd TETGEN ${TetGenLib_FOUND})
	    
if(TetGenLib_FOUND)
  if(NOT TetGenLib_FIND_QUIETLY)
    message(STATUS "FindTetGenLib: done!")
  endif(NOT TetGenLib_FIND_QUIETLY)
else(TetGenLib_FOUND)
  if(TetGenLib_FIND_REQUIRED)
    message(FATAL_ERROR "FindTetGenLib: Could not find TetGenLib libraries")
  endif(TetGenLib_FIND_REQUIRED)
endif(TetGenLib_FOUND)
