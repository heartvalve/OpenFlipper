# - Try to find COMISO
# Once done this will define
#  
#  COMISO_FOUND        - system has COMISO
#  COMISO_INCLUDE_DIR  - the COMISO include directory
#  COMISO_LIBRARY_DIR  - where the libraries are
#  COMISO_LIBRARY      - Link these to use COMISO
#   

IF (COMISO_INCLUDE_DIR)
  # Already in cache, be silent
  SET(COMISO_FIND_QUIETLY TRUE)
ENDIF (COMISO_INCLUDE_DIR)

# Find CoMISo config file
FIND_PATH( COMISO_INCLUDE_DIR CoMISo/Config/config.hh
           PATHS "${CMAKE_SOURCE_DIR}/../" "${CMAKE_SOURCE_DIR}/libs/" )

if ( COMISO_INCLUDE_DIR )

  FILE(READ ${COMISO_INCLUDE_DIR}/CoMISo/Config/config.hh CURRENT_COMISO_CONFIG)

  set(COMISO_OPT_DEPS "")


  STRING(REGEX MATCH "\#define COMISO_MPI_AVAILABLE 1" COMISO_MPI_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_MPI_BUILD_TIME_AVAILABLE )

   find_package(MPI)

   if ( NOT MPI_FOUND )
     message(ERROR "COMISO configured with mpi but mpi not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "MPI")

  endif()

  STRING(REGEX MATCH "\#define COMISO_PETSC_AVAILABLE 1" COMISO_PETSC_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_PETSC_BUILD_TIME_AVAILABLE )

   find_package(PETSC)

   if ( NOT PETSC_FOUND )
     message(ERROR "COMISO configured with petsc but petsc not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "PETSC")

  endif()

  STRING(REGEX MATCH "\#define COMISO_IPOPT_AVAILABLE 1" COMISO_IPOPT_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_IPOPT_BUILD_TIME_AVAILABLE )

   find_package(IPOPT)

   if ( NOT IPOPT_FOUND )
     message(ERROR "COMISO configured with ipopt but ipopt not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "IPOPT")

  endif()

  STRING(REGEX MATCH "\#define COMISO_MUMPS_AVAILABLE 1" COMISO_MUMPS_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_MUMPS_BUILD_TIME_AVAILABLE )

   find_package(MUMPS)

   if ( NOT MUMPS_FOUND )
     message(ERROR "COMISO configured with mumps but mumps not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "MUMPS")

  endif()

  STRING(REGEX MATCH "\#define COMISO_TAO_AVAILABLE 1" COMISO_TAO_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_TAO_BUILD_TIME_AVAILABLE )

   find_package(TAO)

   if ( NOT TAO_FOUND )
     message(ERROR "COMISO configured with tao but tao not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "TAO")
  endif()
  
  STRING(REGEX MATCH "\#define COMISO_TAUCS_AVAILABLE 1" COMISO_TAUCS_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_TAUCS_BUILD_TIME_AVAILABLE )

   find_package(Taucs)

   if ( NOT TAUCS_FOUND )
     message(ERROR "COMISO configured with Taucs but Taucs not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "Taucs")

  endif()

  STRING(REGEX MATCH "\#define COMISO_GUROBI_AVAILABLE 1" COMISO_GUROBI_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_GUROBI_BUILD_TIME_AVAILABLE )

   find_package(GUROBI)

   if ( NOT GUROBI_FOUND )
     message(ERROR "COMISO configured with GUROBI but GUROBI not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "GUROBI")

  endif()


  add_definitions (-DCOMISODLL -DUSECOMISO )

endif(COMISO_INCLUDE_DIR)

IF (COMISO_INCLUDE_DIR)
  SET(COMISO_FOUND TRUE)
  SET( COMISO_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_LIBDIR}" )
  SET( COMISO_LIBRARY "CoMISo")
  SET( COMISO_DEPS "GMM;BLAS;SUITESPARSE" )
ELSE (COMISO_INCLUDE_DIR)
  SET( COMISO_FOUND FALSE )
  SET( COMISO_LIBRARY_DIR )
ENDIF (COMISO_INCLUDE_DIR)

