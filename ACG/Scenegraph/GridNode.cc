/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS GridNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include "GridNode.hh"
#include "SceneGraph.hh"
#include "../GL/gl.hh"
#include <stdio.h>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


GridNode::
GridNode(BaseNode* _parent, const std::string& _name)
  : MaterialNode(_parent, _name),
    horizontalLines_(10),
    verticalLines_(10)
{
}


//-----------------------------------------------------------------------------


unsigned int
GridNode::availableDrawModes() const
{
  return ( DrawModes::WIREFRAME           |
	        DrawModes::SOLID_FLAT_SHADED   );
}


//-----------------------------------------------------------------------------


void
GridNode::boundingBox(Vec3f& /*_bbMin*/, Vec3f& /*_bbMax*/)
{
//   _bbMin.minimize(bb_min_);
//   _bbMax.maximize(bb_max_);
}


//-----------------------------------------------------------------------------


void
GridNode::draw(GLState&  _state  , unsigned int /* _drawMode */ )
{

  glPushAttrib( GL_LIGHTING_BIT ); // STACK_ATTRIBUTES <- LIGHTING_ATTRIBUTE
  glDisable(GL_LIGHTING);

  glPushAttrib( GL_DEPTH_TEST );
  glDisable( GL_DEPTH_TEST );


  _state.push_modelview_matrix();
  _state.reset_modelview();

  _state.push_projection_matrix();
  _state.reset_projection();
  _state.ortho( 0.0, _state.viewport_width() ,0.0,_state.viewport_height(), -1.0 , 1.0 );




  glBegin(GL_LINES);
    for ( int i = 1 ; i <  verticalLines_ ; ++i ) {
      glVertex3f(_state.viewport_width() / verticalLines_ * i ,0.0,0.0);
      glVertex3f(_state.viewport_width() / verticalLines_ * i ,_state.viewport_height(),0.0);
    }

    for ( int i = 1 ; i <  horizontalLines_ ; ++i ) {
      glVertex3f(0.0,0.0 + _state.viewport_height() / horizontalLines_ * i ,0.0);
      glVertex3f(_state.viewport_width(),0.0 + _state.viewport_height() / horizontalLines_ * i,0.0);
    }

  glEnd();

  _state.pop_projection_matrix();
  _state.pop_modelview_matrix();

   glPopAttrib( ); // ->Depth test
   glPopAttrib( ); // ->Lighting
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
