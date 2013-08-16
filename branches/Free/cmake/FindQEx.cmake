#
# Try to find QEx
# Once done this will define
#  
# QEX_FOUND           - system has QEx
# QEX_INCLUDE_DIRS    - the QEx include directories
# QEX_LIBRARIES       - Link these to use QEx
#
# Copyright 2013 Computer Graphics Group, RWTH Aachen University
# Authors: Jan Möbius <moebius@cs.rwth-aachen.de>
#          Hans-Christian Ebke <ebke@cs.rwth-aachen.de>
#
# This file is part of OpenFlipper.
# 
# OpenFlipper is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your
# option) any later version.
# 
# OpenFlipper is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
#   

IF (QEX_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(QEX_FIND_QUIETLY TRUE)
ENDIF (QEX_INCLUDE_DIRS)

FIND_PATH(QEX_INCLUDE_DIR qex.h
	  PATHS /usr/local/include/QEx
                /usr/include/QEx
                "${CMAKE_SOURCE_DIR}/libs/QEx/interfaces/c"
	        )
SET(QEX_LIBRARY_DIR NOTFOUND CACHE PATH "The directory where the QEx libraries can be found.")
SET(SEARCH_PATHS
    "${QEX_INCLUDE_DIR}/../lib"
    "${QEX_LIBRARY_DIR}")
FIND_LIBRARY(QEX_LIBRARY NAMES QEx PATHS ${SEARCH_PATHS})

INCLUDE (FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(QEx DEFAULT_MSG QEX_LIBRARY QEX_INCLUDE_DIR)

IF (QEX_FOUND)
   SET(QEX_LIBRARIES "${QEX_LIBRARY}")
   SET(QEX_INCLUDE_DIRS "${QEX_INCLUDE_DIR}")
ENDIF (QEX_FOUND)

