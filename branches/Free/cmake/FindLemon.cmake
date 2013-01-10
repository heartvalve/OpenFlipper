# - Try to find LEMON
# Once done this will define
#  LEMON_FOUND - System has Lemon
#  LEMON_INCLUDE_DIRS - The Lemon include directories
#  LEMON_LIBRARIES - The libraries needed to use Lemon

if (LEMON_INCLUDE_DIR)
  # in cache already
  set(LEMON_FOUND TRUE)
  set(LEMON_INCLUDE_DIRS "${LEMON_INCLUDE_DIR}" )
  set(LEMON_LIBRARIES "${LEMON_LIBRARY}" )
else (LEMON_INCLUDE_DIR)

find_path(LEMON_INCLUDE_DIR 
          NAMES lemon/min_cost_arborescence.h
          PATHS "$ENV{LEMON_HOME}/include"
          		  "~/sw/lemon/include"
                  "/Library/lemon/mac64/include"
                  "/Library/lemon/include"
                 "C:\\libs\\lemon\\include"
          )

find_library(LEMON_LIBRARY 
             NAMES libemon.a
             PATHS "$ENV{LEMON_HOME}/lib"
             	   "~/sw/lemon/lib"
                   "/Library/lemon/mac64/lib"
                   "C:\\libs\\lemon\\lib"
             )

set(LEMON_INCLUDE_DIRS "${LEMON_INCLUDE_DIR}" )
set(LEMON_LIBRARIES "${LEMON_LIBRARY}" )

message("Found lemon include directory: " ${LEMON_INCLUDE_DIR})
message("Found lemon library: " ${LEMON_LIBRARY})

endif(LEMON_INCLUDE_DIR)
