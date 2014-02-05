/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                            *
 *   $Author$                                                              *
 *   $Date$                                                                *
 *                                                                           *
\*===========================================================================*/


//================================================================
//
//  CLASS OSDTransformNode - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "OSDTransformNode.hh"

#include <ACG/GL/gl.hh>
#include <ACG/Scenegraph/SceneGraph.hh>


//== NAMESPACES ==================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==============================================


void OSDTransformNode::enterOSD( GLState &_state, const DrawModes::DrawMode &_drawMode )
{
	_state.push_modelview_matrix();

	int left, bottom, width, height;
	_state.get_viewport( left, bottom, width, height );

	double aspect = _state.aspect();

	_state.push_projection_matrix();
	_state.reset_projection();

	_state.perspective( 45.0, aspect, 0.8, 20.0 );

	_state.push_modelview_matrix();
	_state.reset_modelview();

	double posX = left   + center_[0] * width;
	double posY = bottom + center_[1] * height;

	ACG::Vec3d pos = _state.unproject( ACG::Vec3d( posX, posY, 0.1 ) );

	_state.pop_modelview_matrix();

	GLMatrixd modelview = _state.modelview();

	modelview(0,3) = 0.0;
	modelview(1,3) = 0.0;
	modelview(2,3) = 0.0;

	_state.set_modelview( modelview );
	_state.translate( pos[0], pos[1], pos[2], MULT_FROM_LEFT );
}


//----------------------------------------------------------------


void OSDTransformNode::leaveOSD( GLState &_state, const DrawModes::DrawMode &_drawMode )
{
	_state.pop_projection_matrix();
	_state.pop_modelview_matrix();
}


//================================================================


} // namespace SceneGraph
} // namespace ACG
