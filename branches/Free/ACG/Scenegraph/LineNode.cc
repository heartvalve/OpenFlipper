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
//  CLASS LineNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "LineNode.hh"
#include "../GL/gl.hh"


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================


void
LineNode::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  ConstPointIter p_it=points_.begin(), p_end=points_.end();

  if ( p_it!=p_end ){
    _bbMax = *p_it;
    _bbMin = *p_it;
    ++p_it;
  }

  for (; p_it!=p_end; ++p_it)
  {
      _bbMax.maximize(*p_it);
      _bbMin.minimize(*p_it);
  }
}


//----------------------------------------------------------------------------


DrawModes::DrawMode
LineNode::
availableDrawModes() const
{
  return DrawModes::WIREFRAME;
}


//----------------------------------------------------------------------------

void
LineNode::
enter(GLState& _state , const DrawModes::DrawMode& _drawMode)
{
    MaterialNode::enter(_state, _drawMode);
    
    if (alwaysOnTop()) {
	//store current depth comparison function (needed for lasso selection)
	prev_depth_ = _state.depthFunc();
	
	//set depth function and change GLState accordingly
	_state.set_depthFunc(GL_ALWAYS);
    }
}

//----------------------------------------------------------------------------

void
LineNode::
draw(GLState& /* _state */ , const DrawModes::DrawMode& _drawMode)
{
  if (_drawMode & DrawModes::WIREFRAME)
  {
    glDisable(GL_LIGHTING);

    if (line_mode_ == LineSegmentsMode)
      glBegin(GL_LINES);
    else
      glBegin(GL_LINE_STRIP);

    if ((line_mode_ == LineSegmentsMode) && (points_.size()/2 == colors_.size()) )
    {
      ConstPointIter p_it=points_.begin(), p_end=points_.end();
      ConstColorIter c_it=colors_.begin();

      Color c((char)255, (char)255, (char)255);
      if(c_it != colors_.end()) {
        c = *c_it;
      }
      
      int cnt = 0;
      for (; p_it!=p_end; ++p_it)
      {
        if ((cnt > 0) && (cnt % 2 == 0) && (c_it+1) != colors_.end()) {
          ++c_it;
          c = *c_it;
        }
        
        glColor(c);
        glVertex(*p_it);

        ++cnt;
      }
    }
    else
    {
      ConstPointIter p_it=points_.begin(), p_end=points_.end();
      for (; p_it!=p_end; ++p_it)
        glVertex(*p_it);
    }

    glEnd();
  }
}
  
//----------------------------------------------------------------------------

void
LineNode::
leave(GLState& _state , const DrawModes::DrawMode& _drawMode)
{
  if (alwaysOnTop()) {
    //restore depth function and change GLState accordingly
    _state.set_depthFunc(prev_depth_);
  }

  MaterialNode::leave(_state, _drawMode);
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
