/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  Global defines for OpenFlipper
//
//=============================================================================

/**
 * \file GlobalDefines.hh
 * This File contains the required defines for the OpenFlipper Framework
 */

#ifndef GLOBALDEFINES_HH
#define GLOBALDEFINES_HH


/**
 * Get GCC version and convert to MMmm where MM is the major
 * and mm the minor release version.
 * For example if GCC_VERSION == 4030, we consider gcc-4.3.
 */

#define GCC_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__ * 10)


#if QT_VERSION > 0x050101
  #ifdef WIN32
    #pragma message("Detected QT Version higher than 5.1.1!")
    #pragma message("OpenFlipper has not been tested with this QT Version.")
  #else
    #warning Detected QT Version higher than 4.8.4!
    #warning OpenFlipper has not been tested with this QT Version.
  #endif
#endif

//== DEEFINES =================================================================

/** Force static casts for Properties in OpenMesh.
 * Cross Plugin dynamic casts are not possible due to locally defined symbols
 */
#define OM_FORCE_STATIC_CAST

/** Enable openmp where possible (for gcc version >= 4.3)
 */
#ifndef USE_OPENMP
  #if GCC_VERSION >= 4030
    #define USE_OPENMP
  #endif
#endif


#ifndef DLLEXPORT
	#ifdef WIN32
		#ifdef PLUGINLIBDLL
			#ifdef USEPLUGINLIBDLL
				#define DLLEXPORT __declspec(dllimport)
				#define DLLEXPORTONLY
			#else
				#define DLLEXPORT __declspec(dllexport)
				#define DLLEXPORTONLY __declspec(dllexport)
			#endif
		#else
			#define DLLEXPORT
			#define DLLEXPORTONLY
		#endif
	#else
		#define DLLEXPORT
		#define DLLEXPORTONLY
	#endif
#endif

// Disable the warnings about needs to have DLL interface as we have tons of vector templates
#ifdef WIN32
  #pragma warning( disable: 4251 )
#endif


//=============================================================================
#endif // GLOBALDEFINES_HH defined
//=============================================================================
