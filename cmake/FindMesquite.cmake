# - Try to find Mesquite
# Once done this will define
#  
#  MESQUITE_FOUND        - system has MESQUITE
#  MESQUITE_INCLUDE_DIR  - the MESQUITE include directory
#  MESQUITE_LIBRARY_DIR  - where the libraries are
#  MESQUITE_LIBRARY      - Link these to use MESQUITE
#   

IF(MESQUITE_INCLUDE_DIR)
  # Already in cache, be silent
  SET(MESQUITE_FIND_QUIETLY TRUE)
ENDIF(MESQUITE_INCLUDE_DIR)

if(WIN32)
   if(MSVC80)
       set(COMPILER_PATH "C:/Program\ Files/Microsoft\ Visual\ Studio\ 8/VC")
   endif(MSVC80)
   if(MSVC71)
       set(COMPILER_PATH "C:/Program\ Files/Microsoft\ Visual\ Studio\ .NET\ 2003/Vc7")
   endif(MSVC71)
   FIND_PATH(MESQUITE_INCLUDE_DIR Mesquite_all_headers.hpp
              PATHS "c:/mesquite/include" "c:/libs/mesquite/include" "c:/libs/mesquite-2.1.4/include" ${COMPILER_PATH}/PlatformSDK/Include)
   SET(MESQUITE_NAMES mesquite)
   FIND_LIBRARY(MESQUITE_LIBRARY
                 NAMES ${MESQUITE_NAMES}
                 PATHS "c:/mesquite/lib"  "c:/libs/mesquite/lib" "c:/libs/mesquite-2.1.4/lib" ${COMPILER_PATH}/PlatformSDK/Lib)
else(WIN32)
   FIND_PATH(MESQUITE_INCLUDE_DIR Mesquite_all_headers.hpp
              PATHS /usr/local/include /usr/include)
   SET(MESQUITE_NAMES mesquite MESQUITE)
   FIND_LIBRARY(MESQUITE_LIBRARY
                 NAMES ${MESQUITE_NAMES}
                 PATHS /usr/lib /usr/local/lib)
endif(WIN32)

GET_FILENAME_COMPONENT(MESQUITE_LIBRARY_DIR ${MESQUITE_LIBRARY} PATH)

IF(MESQUITE_INCLUDE_DIR AND MESQUITE_LIBRARY)
   SET(MESQUITE_FOUND TRUE)
ELSE(MESQUITE_INCLUDE_DIR AND MESQUITE_LIBRARY)
   SET(MESQUITE_FOUND FALSE)
   SET(MESQUITE_LIBRARY_DIR)
ENDIF(MESQUITE_INCLUDE_DIR AND MESQUITE_LIBRARY)

