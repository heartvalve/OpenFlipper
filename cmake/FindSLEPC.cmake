# - Try to find SLEPSc
# Once done this will define
#
#  SLEPC_FOUND        - system has SLEPSc
#  SLEPC_INCLUDES     - the SLEPSc include directories
#  SLEPC_LIBRARIES    - Link these to use SLEPSc
#  SLEPC_COMPILER     - Compiler used by SLEPSc, helpful to find a compatible MPI
#  SLEPC_DEFINITIONS  - Compiler switches for using SLEPSc
#  SLEPC_MPIEXEC      - Executable for running MPI programs
#
# Setting these changes the behavior of the search
#  SLEPC_DIR - directory in which SLEPSc resides
#  SLEPC_ARCH - build architecture
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




