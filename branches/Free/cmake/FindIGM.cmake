#
# Try to find IGM
# Once done this will define
#  
# IGM_FOUND           - system has IGM
# IGM_INCLUDE_DIRS    - the IGM include directories
# IGM_LIBRARIES       - Link these to use IGM
# IGM_LIBRARY_DIR     - directory where the libraries are included
#
# Copyright 2013 Computer Graphics Group, RWTH Aachen University
# Authors: David Bommes <bommes@cs.rwth-aachen.de>
#

cmake_minimum_required(VERSION 2.8.9)

#if already found via finder or simulated finder in openmesh CMakeLists.txt, skip the search
IF (NOT IGM_FOUND) 
  SET (SEARCH_PATHS 
    /usr/local/
    /usr/
    "${CMAKE_SOURCE_DIR}/IGM/src/"
    "${CMAKE_SOURCE_DIR}/libs/IGM/src/"
    "${CMAKE_SOURCE_DIR}/../IGM/src/"
  )

  FIND_PATH (IGM_INCLUDE_DIR IGM/QuadMeshGeneratorT.hh
    PATHS ${SEARCH_PATHS}
    PATH_SUFFIXES include)

  FIND_LIBRARY(IGM_LIBRARY NAMES IGM
    PATHS ${SEARCH_PATHS}
    PATH_SUFFIXES lib lib64)


  set(IGM_LIBRARIES ${IGM_LIBRARY})
  set(IGM_INCLUDE_DIRS ${IGM_INCLUDE_DIR} )

#checks, if OPENMESH was found and sets OPENMESH_FOUND if so
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(IGM  DEFAULT_MSG
                                    IGM_LIBRARY IGM_INCLUDE_DIR)
 
#sets the library dir 
  get_filename_component(_IGM_LIBRARY_DIR ${IGM_LIBRARY} PATH)
  set (IGM_LIBRARY_DIR "${_IGM_LIBRARY_DIR}" CACHE PATH "The directory where the IGM libraries can be found.")
  
 
  mark_as_advanced(IGM_INCLUDE_DIR IGM_LIBRARY IGM_LIBRARY_DIR)
endif()
