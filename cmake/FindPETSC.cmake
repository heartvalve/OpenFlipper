# - Try to find PETSc
# Once done this will define
#
#  PETSC_FOUND        - system has PETSc
#  PETSC_INCLUDES     - the PETSc include directories
#  PETSC_LIBRARIES    - Link these to use PETSc
#  PETSC_COMPILER     - Compiler used by PETSc, helpful to find a compatible MPI
#  PETSC_DEFINITIONS  - Compiler switches for using PETSc
#  PETSC_MPIEXEC      - Executable for running MPI programs
#
# Setting these changes the behavior of the search
#  PETSC_DIR - directory in which PETSc resides
#  PETSC_ARCH - build architecture
#
# Redistribution and use is allowed according to the terms of the BSD license.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products 
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

find_path (PETSC_DIR include/petsc.h
  HINTS ENV PETSC_DIR
  PATHS /usr/lib/petscdir/3.0.0
        /opt/local/lib/petsc
        "C:\\libs\\petsc-3.0.0-p6"
  DOC "PETSc Directory")


if (PETSC_DIR AND NOT PETSC_ARCH)
  set (_petsc_arches
    $ENV{PETSC_ARCH}                   # If set, use environment variable first
    linux-gnu-c-debug linux-gnu-c-opt  # Debian defaults
    cygwin-c-opt                       # Windows version
    x86_64-unknown-linux-gnu i386-unknown-linux-gnu)
  set (petscconf "NOTFOUND" CACHE FILEPATH "Cleared" FORCE)
  foreach (arch ${_petsc_arches})
    if (NOT PETSC_ARCH)
      find_path (petscconf petscconf.h
	HINTS ${PETSC_DIR}
	PATH_SUFFIXES ${arch}/include bmake/${arch}
	NO_DEFAULT_PATH)
      if (petscconf)
	set (PETSC_ARCH "${arch}" CACHE STRING "PETSc build architecture")
      endif (petscconf)
    endif (NOT PETSC_ARCH)
  endforeach (arch)
  set (petscconf "NOTFOUND" CACHE INTERNAL "Scratch variable" FORCE)
endif (PETSC_DIR AND NOT PETSC_ARCH)


set (petsc_slaves LIBRARIES_SYS LIBRARIES_VEC LIBRARIES_MAT LIBRARIES_DM LIBRARIES_KSP LIBRARIES_SNES LIBRARIES_TS)
include (FindPackageMultipass)
find_package_multipass (PETSc petsc_config_current
  STATES DIR ARCH
  DEPENDENTS INCLUDES LIBRARIES COMPILER MPIEXEC ${petsc_slaves})

# Determine whether the PETSc layout is old-style (through 2.3.3) or
# new-style (not yet released, petsc-dev)
if (EXISTS ${PETSC_DIR}/${PETSC_ARCH}/include/petscconf.h)   # > 2.3.3
  set (petsc_conf_base ${PETSC_DIR}/conf/base)
  set (PETSC_VERSION "3.0.0")
elseif (EXISTS ${PETSC_DIR}/bmake/${PETSC_ARCH}/petscconf.h) # <= 2.3.3
  set (petsc_conf_base ${PETSC_DIR}/bmake/common/base)
  set (PETSC_VERSION "2.3.3")
else (EXISTS ${PETSC_DIR}/bmake/${PETSC_ARCH}/petscconf.h)
  set (petsc_conf_base "NOTFOUND")
endif (EXISTS ${PETSC_DIR}/${PETSC_ARCH}/include/petscconf.h)

if (petsc_conf_base AND NOT petsc_config_current)

  # Put variables into environment since they are needed to get
  # configuration (petscvariables) in the PETSc makefile
  set (ENV{PETSC_DIR} ${PETSC_DIR})
  set (ENV{PETSC_ARCH} ${PETSC_ARCH})

  IF (NOT WIN32)
  	# A temporary makefile to probe the PETSc configuration
  	set (petsc_config_makefile ${PROJECT_BINARY_DIR}/Makefile.petsc)
  	file (WRITE ${petsc_config_makefile}
"## This file was autogenerated by FindPETSc.cmake
# PETSC_DIR  = ${PETSC_DIR}
# PETSC_ARCH = ${PETSC_ARCH}
include ${petsc_conf_base}
show :
	-@echo -n \${\${VARIABLE}}
")

  	macro (PETSC_GET_VARIABLE name var)
    		set (${var} "NOTFOUND" CACHE INTERNAL "Cleared" FORCE)
    		execute_process (COMMAND ${CMAKE_MAKE_PROGRAM} -f ${petsc_config_makefile} show VARIABLE=${name}
      	OUTPUT_VARIABLE ${var}
      	RESULT_VARIABLE petsc_return)
  	endmacro (PETSC_GET_VARIABLE)

  	petsc_get_variable (PETSC_LIB_DIR            petsc_lib_dir)
  	petsc_get_variable (PETSC_EXTERNAL_LIB_BASIC petsc_libs_external)
  	petsc_get_variable (PETSC_CCPPFLAGS          petsc_cpp_line)
  	petsc_get_variable (PETSC_INCLUDE            petsc_include)
  	petsc_get_variable (PCC                      petsc_cc)
  	petsc_get_variable (MPIEXEC                  petsc_mpiexec)

  	# We are done with the temporary Makefile, calling PETSC_GET_VARIABLE after this point is invalid!
  	file (REMOVE ${petsc_config_makefile})

  	include (ResolveCompilerPaths)
  	# Extract include paths and libraries from compile command line
  	resolve_includes (petsc_includes_all "${petsc_cpp_line}")

  	message (STATUS "petsc_lib_dir ${petsc_lib_dir}")

  	macro (PETSC_FIND_LIBRARY suffix name)
    		set (PETSC_LIBRARY_${suffix} "NOTFOUND" CACHE INTERNAL "Cleared" FORCE)
    	find_library (PETSC_LIBRARY_${suffix} NAMES ${name} HINTS ${petsc_lib_dir})
    	set (PETSC_LIBRARIES_${suffix} "${PETSC_LIBRARY_${suffix}}")
    	mark_as_advanced (PETSC_LIBRARY_${suffix})
  	endmacro (PETSC_FIND_LIBRARY suffix name)
  	petsc_find_library (SYS  petsc)
  	petsc_find_library (VEC  petscvec)
  	petsc_find_library (MAT  petscmat)
  	petsc_find_library (DM   petscdm)
  	petsc_find_library (KSP  petscksp)
  	petsc_find_library (SNES petscsnes)
  	petsc_find_library (TS   petscts)

  	include (CheckCSourceRuns)
  	macro (PETSC_TEST_RUNS includes libraries runs)
    		multipass_c_source_runs (PETSc "${includes}" "${libraries}" "
static const char help[] = \"PETSc test program.\";
#include \"petscts.h\"
int main(int argc,char *argv[]) {
  PetscErrorCode ierr;
  TS ts;
  PetscFunctionBegin;
  ierr = PetscInitialize(&argc,&argv,0,help);CHKERRQ(ierr);
  ierr = TSCreate(PETSC_COMM_WORLD,&ts);CHKERRQ(ierr);
  ierr = TSSetFromOptions(ts);CHKERRQ(ierr);
  ierr = TSDestroy(ts);CHKERRQ(ierr);
  ierr = PetscFinalize();CHKERRQ(ierr);
  PetscFunctionReturn(0);
}
" ${runs})
    if (${${runs}})
      set (PETSC_EXECUTABLE_RUNS "YES" CACHE BOOL
	"Can the system successfully run a PETSc executable?  This variable can be manually set to \"YES\" to force CMake to accept a given PETSc configuration, but this will almost always result in a broken build.  If you change PETSC_DIR, PETSC_ARCH, or PETSC_CURRENT you would have to reset this variable." FORCE)
    endif (${${runs}})
  	endmacro (PETSC_TEST_RUNS)

  	macro (PETSC_JOIN libs deps)
    	list (APPEND PETSC_LIBRARIES_${libs} ${PETSC_LIBRARIES_${deps}})
  	endmacro (PETSC_JOIN libs deps)
  	petsc_join (VEC  SYS)
  	petsc_join (MAT  VEC)
  	petsc_join (DM   MAT)
  	petsc_join (KSP  DM)
  	petsc_join (SNES KSP)
  	petsc_join (TS   SNES)
  	petsc_join (ALL  TS)
  ENDIF(NOT WIN32)

  MESSAGE(STATUS "Pos 1")

  find_path (PETSC_INCLUDE_DIR petscts.h HINTS "${PETSC_DIR}" PATH_SUFFIXES include NO_DEFAULT_PATH)
  find_path (PETSC_INCLUDE_CONF petscconf.h HINTS "${PETSC_DIR}" PATH_SUFFIXES "${PETSC_ARCH}/include" "bmake/${PETSC_ARCH}" NO_DEFAULT_PATH)
  mark_as_advanced (PETSC_INCLUDE_DIR PETSC_INCLUDE_CONF)

  IF (NOT WIN32)
   	set (petsc_includes_minimal ${PETSC_INCLUDE_CONF} ${PETSC_INCLUDE_DIR})

  	petsc_test_runs ("${petsc_includes_minimal}" "${PETSC_LIBRARIES_TS}" petsc_works_minimal)
  	if (petsc_works_minimal)
    		message (STATUS "Minimal PETSc includes and libraries work.  This probably means we are building with shared libs.")
    		set (petsc_includes_needed "${petsc_includes_minimal}")
  	else (petsc_works_minimal)	# Minimal includes fail, see if just adding full includes fixes it
    		petsc_test_runs ("${petsc_includes_all}" "${PETSC_LIBRARIES_TS}" petsc_works_allincludes)
    		if (petsc_works_allincludes) # It does, we just need all the includes (
      		message (STATUS "PETSc requires extra include paths, but links correctly with only interface libraries.  This is an unexpected configuration (but it seems to work fine).")
      		set (petsc_includes_needed ${petsc_includes_all})
    		else (petsc_works_allincludes) # We are going to need to link the external libs explicitly
      		resolve_libraries (petsc_libraries_external "${petsc_libs_external}")
      		foreach (pkg SYS VEC MAT DM KSP SNES TS ALL)
				list (APPEND PETSC_LIBRARIES_${pkg}  ${petsc_libraries_external})
      		endforeach (pkg)
      		petsc_test_runs ("${petsc_includes_minimal}" "${PETSC_LIBRARIES_TS}" petsc_works_alllibraries)
      		if (petsc_works_alllibraries)
	 			message (STATUS "PETSc only need minimal includes, but requires explicit linking to all dependencies.  This is expected when PETSc is built with static libraries.")
				set (petsc_includes_needed ${petsc_includes_minimal})
      		else (petsc_works_alllibraries)
				# It looks like we really need everything, should have listened to Matt
				set (petsc_includes_needed ${petsc_includes_all})
				petsc_test_runs ("${petsc_includes_all}" "${PETSC_LIBRARIES_TS}" petsc_works_all)
				if (petsc_works_all) # We fail anyways
	 				message (STATUS "PETSc requires extra include paths and explicit linking to all dependencies.  This probably means you have shared libraries and something unexpected in PETSc headers.")
				else (petsc_works_all) # We fail anyways
	  				message (STATUS "PETSc could not be used, maybe the install is broken.")
				endif (petsc_works_all)
      		endif (petsc_works_alllibraries)
    		endif (petsc_works_allincludes)
  	endif (petsc_works_minimal)

   	# We do an out-of-source build so __FILE__ will be an absolute path, hence __SDIR__ is superfluous
  	set (PETSC_DEFINITIONS "-D__SDIR__=\"\"" CACHE STRING "PETSc definitions" FORCE)
  	# Sometimes this can be used to assist FindMPI.cmake
  	set (PETSC_MPIEXEC ${petsc_mpiexec} CACHE FILEPATH "Executable for running PETSc MPI programs" FORCE)
  	set (PETSC_INCLUDES ${petsc_includes_needed} CACHE STRING "PETSc include path" FORCE)
  	set (PETSC_LIBRARIES ${PETSC_LIBRARIES_ALL} CACHE STRING "PETSc libraries" FORCE)
  	set (PETSC_COMPILER ${petsc_cc} CACHE FILEPATH "PETSc compiler" FORCE)
  	# Note that we have forced values for all these choices.  If you
  	# change these, you are telling the system to trust you that they
  	# work.  It is likely that you will end up with a broken build.
  	mark_as_advanced (PETSC_INCLUDES PETSC_LIBRARIES PETSC_COMPILER PETSC_DEFINITIONS PETSC_MPIEXEC PETSC_EXECUTABLE_RUNS)

  ELSE (NOT WIN32)
    	# We do an out-of-source build so __FILE__ will be an absolute path, hence __SDIR__ is superfluous
  	set (PETSC_DEFINITIONS "-D__SDIR__=\"\"" CACHE STRING "PETSc definitions" FORCE)
  	# Sometimes this can be used to assist FindMPI.cmake
  	set (PETSC_MPIEXEC ${petsc_mpiexec} CACHE FILEPATH "Executable for running PETSc MPI programs" FORCE)
      set (PETSC_INCLUDES "${PETSC_INCLUDE_CONF} ${PETSC_INCLUDE_DIR} " CACHE STRING "PETSc include path" FORCE)
      set (PETSC_INCLUDE_DIRS "${PETSC_INCLUDE_CONF}" CACHE STRING "PETSc include paths" FORCE)
      list (APPEND PETSC_INCLUDE_DIRS "${PETSC_INCLUDE_DIR}" )

  	set (PETSC_COMPILER ${petsc_cc} CACHE FILEPATH "PETSc compiler" FORCE)
  	# Note that we have forced values for all these choices.  If you
  	# change these, you are telling the system to trust you that they
  	# work.  It is likely that you will end up with a broken build.
  	mark_as_advanced (PETSC_INCLUDES PETSC_LIBRARIES PETSC_COMPILER PETSC_DEFINITIONS PETSC_MPIEXEC PETSC_EXECUTABLE_RUNS)   
  ENDIF(NOT WIN32)

  message(STATUS "Libraries for PETSC : ${PETSC_LIBRARIES}")
endif (petsc_conf_base AND NOT petsc_config_current)


if ( NOT WIN32 )
	include (FindPackageHandleStandardArgs)
		find_package_handle_standard_args (PETSc
  			"PETSc could not be found.  Be sure to set PETSC_DIR and PETSC_ARCH."
  			PETSC_INCLUDES PETSC_LIBRARIES PETSC_EXECUTABLE_RUNS)
else ( NOT WIN32 )
	if ( PETSC_INCLUDE_DIR )
		set( PETSC_FOUND TRUE)
      endif( PETSC_INCLUDE_DIR  )
      set(PETSC_LIBRARIES  "" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libpetsc.lib" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libpetsccontrib.lib" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libpetscdm.lib" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libpetscksp.lib" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libpetscmat.lib" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libpetscsnes.lib" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libpetscts.lib" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libpetscvec.lib" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libf2clapack.lib" )
  	list (APPEND PETSC_LIBRARIES "${PETSC_DIR}/${PETSC_ARCH}/lib/libf2cblas.lib" )
message(STATUS ${PETSC_LIBRARIES})
endif ( NOT WIN32 )
message(STATUS ${PETSC_LIBRARIES})
