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
//
//=============================================================================


#ifndef DRAWMODECONVERTER_HH
#define DRAWMODECONVERTER_HH

//== INCLUDES =================================================================

#include <ACG/Scenegraph/DrawModes.hh> 
#include <QString>

//== DEFINES =================================================================

#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

DLLEXPORT
std::vector< QString > drawModeToList(unsigned int _drawMode);

DLLEXPORT
unsigned int ListToDrawMode ( std::vector< QString > _draw_modes );


//=============================================================================
#endif // DRAWMODECONVERTER_HH defined
//=============================================================================
