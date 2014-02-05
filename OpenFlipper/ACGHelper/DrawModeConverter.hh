/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
//
//=============================================================================


#ifndef DRAWMODECONVERTER_HH
#define DRAWMODECONVERTER_HH

//== INCLUDES =================================================================

#include <ACG/Scenegraph/DrawModes.hh> 
#include <QString>
#include <OpenFlipper/common/GlobalDefines.hh>

//== DEFINES =================================================================


//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** \brief convert a drawMode to a List of the drawmode names
*
*/
DLLEXPORT
std::vector< QString > drawModeToList(ACG::SceneGraph::DrawModes::DrawMode _drawMode);

/** \brief convert a drawMode name list to a drawmode
*
*/
DLLEXPORT
ACG::SceneGraph::DrawModes::DrawMode listToDrawMode ( std::vector< QString > _draw_modes );

/** \brief convert a drawMode to a List of the drawmode readable descriptions
*
*/
DLLEXPORT
std::vector< QString > drawModeToDescriptions(ACG::SceneGraph::DrawModes::DrawMode _drawMode);

/** \brief convert a  List of the drawmode readable descriptions to a drawMode
*
*/
DLLEXPORT
ACG::SceneGraph::DrawModes::DrawMode descriptionsToDrawMode ( std::vector< QString > _draw_modes );

//=============================================================================
#endif // DRAWMODECONVERTER_HH defined
//=============================================================================
