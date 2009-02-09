//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




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


//== DEEFINES =================================================================

/** Force static casts for Properties in OpenMesh.
 * Cross Plugin dynamic casts are not possible due to locally defined symbols
 */
#define OM_FORCE_STATIC_CAST

/** Enable openmp where possible (for gcc version >= 4.3)
 */
#if GCC_VERSION >= 4030
  #define USE_OPENMP
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


//=============================================================================
#endif // GLOBALDEFINES_HH defined
//=============================================================================
