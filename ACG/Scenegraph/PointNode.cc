/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS PointNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "PointNode.hh"
#include "../Utils/StopWatch.hh"
#include "../GL/gl.hh"

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


void
PointNode::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  ConstPointIter p_it=points_.begin(), p_end=points_.end();
  for (; p_it!=p_end; ++p_it) {
    _bbMin.minimize(*p_it); 
    _bbMax.maximize(*p_it);
  }
}


//----------------------------------------------------------------------------

  
  DrawModes::DrawMode 
PointNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS | 
	   DrawModes::POINTS_SHADED | 
	   DrawModes::POINTS_COLORED );
}


//----------------------------------------------------------------------------


void
PointNode::
draw(GLState& /* _state */ , const DrawModes::DrawMode& _drawMode)
{
  if (points_.size() == 0)
    return;
  
  // points
  if (_drawMode & DrawModes::POINTS)
  {
    ACG::GLState::disable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(&points_[0]);
    glDrawArrays(GL_POINTS, 0, points_.size());
  }


  // points and normals
  if (_drawMode & DrawModes::POINTS_SHADED)
  {
    if (points_.size() == normals_.size())
    {
      ACG::GLState::enable(GL_LIGHTING);
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(&points_[0]);
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(&normals_[0]);
      glDrawArrays(GL_POINTS, 0, points_.size());
    }
  }


  // points and colors
  if (_drawMode & DrawModes::POINTS_COLORED)
  {
    if (points_.size() == colors_.size())
    {
      ACG::GLState::disable(GL_LIGHTING);
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(&points_[0]);
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(&colors_[0]);
      glDrawArrays(GL_POINTS, 0, points_.size());
    }
  }


  // disable arrays
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
