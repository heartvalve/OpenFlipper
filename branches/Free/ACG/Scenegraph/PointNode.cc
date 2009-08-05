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
boundingBox(Vec3f& _bbMin, Vec3f& _bbMax)
{
  ConstPointIter p_it=points_.begin(), p_end=points_.end();
  for (; p_it!=p_end; ++p_it) {
    _bbMin.minimize(*p_it); _bbMax.maximize(*p_it);
  }
}


//----------------------------------------------------------------------------

  
unsigned int 
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
draw(GLState& /* _state */ , unsigned int _drawMode)
{
  glDepthFunc(depthFunc());


  // points
  if (_drawMode & DrawModes::POINTS)
  {
    glDisable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &points_[0]);
    glDrawArrays(GL_POINTS, 0, points_.size());
  }


  // points and normals
  if (_drawMode & DrawModes::POINTS_SHADED)
  {
    if (points_.size() == normals_.size())
    {
      glEnable(GL_LIGHTING);
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, &points_[0]);
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, &normals_[0]);
      glDrawArrays(GL_POINTS, 0, points_.size());
    }
  }


  // points and colors
  if (_drawMode & DrawModes::POINTS_COLORED)
  {
    if (points_.size() == colors_.size())
    {
      glDisable(GL_LIGHTING);
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, &points_[0]);
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(3, GL_UNSIGNED_BYTE, 0, &colors_[0]);
      glDrawArrays(GL_POINTS, 0, points_.size());
    }
  }


  // disable arrays
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  glDepthFunc(GL_LESS);
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
